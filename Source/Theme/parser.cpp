#include "parser.hpp"
#include "security.hpp"

#include <QDebug>
#include <QFile>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace Theme {

namespace {

static void instructionLimitHook(lua_State* L, lua_Debug* ar) {
    Q_UNUSED(ar);
    luaL_error(L, "Lua script instruction limit exceeded (freeze/loop detected)");
}

QString getStringField(lua_State* L, const char* key, const QString& def = "") {
    QString res = def;
    lua_pushstring(L, key);
    if (lua_gettable(L, -2) == LUA_TSTRING) {
        res = QString::fromUtf8(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    return res;
}

double getNumberField(lua_State* L, const char* key, double def = 0.0) {
    double res = def;
    lua_pushstring(L, key);
    if (lua_gettable(L, -2) == LUA_TNUMBER) {
        res = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return res;
}

bool getBoolField(lua_State* L, const char* key, bool def = false) {
    bool res = def;
    lua_pushstring(L, key);
    int type = lua_gettable(L, -2);
    if (type == LUA_TBOOLEAN) {
        res = lua_toboolean(L, -1);
    } else if (type == LUA_TSTRING) {
        QString s = QString::fromUtf8(lua_tostring(L, -1)).toLower();
        res = (s == "true" || s == "infinite");
    }
    lua_pop(L, 1);
    return res;
}

QVariant getValueField(lua_State* L, const char* key) {
    QVariant res;
    lua_pushstring(L, key);
    int type = lua_gettable(L, -2);
    if (type == LUA_TNUMBER) {
        res = lua_tonumber(L, -1);
    } else if (type == LUA_TSTRING) {
        res = QString::fromUtf8(lua_tostring(L, -1));
    } else if (type == LUA_TBOOLEAN) {
        res = static_cast<bool>(lua_toboolean(L, -1));
    }
    lua_pop(L, 1);
    return res;
}

QVariantList getListField(lua_State* L, const char* key) {
    QVariantList list;
    lua_pushstring(L, key);
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        const int len = static_cast<int>(luaL_len(L, -1));
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (lua_isnumber(L, -1)) {
                list.append(lua_tonumber(L, -1));
            } else if (lua_isstring(L, -1)) {
                list.append(QString::fromUtf8(lua_tostring(L, -1)));
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    return list;
}

QVariantMap getMapField(lua_State* L, const char* key) {
    QVariantMap map;
    lua_pushstring(L, key);
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2)) {
                const QString k = QString::fromUtf8(lua_tostring(L, -2));
                if (lua_isnumber(L, -1)) {
                    map[k] = lua_tonumber(L, -1);
                } else if (lua_isstring(L, -1)) {
                    map[k] = QString::fromUtf8(lua_tostring(L, -1));
                } else if (lua_isboolean(L, -1)) {
                    map[k] = static_cast<bool>(lua_toboolean(L, -1));
                } else if (lua_istable(L, -1)) {
                    QVariantList subList;
                    const int subLen = static_cast<int>(luaL_len(L, -1));
                    for (int i = 1; i <= subLen; ++i) {
                        lua_rawgeti(L, -1, i);
                        subList.append(QString::fromUtf8(lua_tostring(L, -1)));
                        lua_pop(L, 1);
                    }
                    map[k] = subList;
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    return map;
}

ThemeDefinition extractFromTable(lua_State* L) {
    ThemeDefinition theme;
    if (!lua_istable(L, -1)) return theme;

    lua_pushstring(L, "meta");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        theme.header.themeName = Security::sanitizeString(getStringField(L, "name", "Default"));
        theme.header.author = Security::sanitizeString(getStringField(L, "author", "Unknown"));
        theme.header.version = Security::sanitizeString(getStringField(L, "version", "1.0"));
        theme.header.description = Security::sanitizeString(getStringField(L, "description", ""));
    }
    lua_pop(L, 1);

    QString font = getStringField(L, "font", "");
    if (font.isEmpty()) font = getStringField(L, "font_family", "");
    theme.fontFamily = Security::sanitizeString(font);

    QString bg = getStringField(L, "bg_image", "");
    if (bg.isEmpty()) bg = getStringField(L, "bg", "");
    if (Security::isPathSafe(bg)) {
        theme.bgImage = bg;
    }

    lua_pushstring(L, "colors");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
                QString key = QString::fromUtf8(lua_tostring(L, -2)).trimmed().toLower();
                QString val = QString::fromUtf8(lua_tostring(L, -1)).trimmed();
                if (QColor::isValidColorName(val)) {
                    theme.colors[key] = QColor(val);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_pushstring(L, "geometry");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1)) {
                QString key = QString::fromUtf8(lua_tostring(L, -2)).trimmed().toLower();
                theme.geometry[key] = lua_tonumber(L, -1);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_pushstring(L, "text");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
                QString key = QString::fromUtf8(lua_tostring(L, -2)).trimmed().toLower();
                QString val = QString::fromUtf8(lua_tostring(L, -1)).trimmed();
                theme.textOverrides[key] = Security::sanitizeString(val);
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    theme.effects = getMapField(L, "effects");
    theme.borders = getMapField(L, "borders");

    lua_pushstring(L, "animations");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        const int len = static_cast<int>(luaL_len(L, -1));
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -1, i);
            if (lua_istable(L, -1)) {
                ThemeAnimation a;
                a.name = getStringField(L, "name", QString("anim_%1").arg(i));
                a.target = getStringField(L, "target", "");
                a.property = getStringField(L, "property", "");
                a.fromValue = getValueField(L, "from");
                a.toValue = getValueField(L, "to");
                a.duration = static_cast<int>(getNumberField(L, "duration", 250));
                a.loop = getBoolField(L, "loop", false);
                a.trigger = getStringField(L, "trigger", "none").toLower();
                a.easing = getStringField(L, "easing", "spring").toLower();
                a.direction = getStringField(L, "direction", "alternate").toLower();
                a.delay = static_cast<int>(getNumberField(L, "delay", 0));
                a.overshoot = static_cast<float>(getNumberField(L, "overshoot", 1.45));
                a.bezier = getListField(L, "bezier");

                if (!a.target.isEmpty() && !a.property.isEmpty()) {
                    theme.animations.push_back(a);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    return theme;
}

} // namespace

ThemeDefinition Parser::parseScript(const QString& luaCode) {
    lua_State* L = luaL_newstate();
    if (!L) return {};

    luaL_requiref(L, "_G", luaopen_base, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(L, 1);

    const char* dangerousGlobals[] = {
        "dofile", "loadfile", "load", "loadstring",
        "require", "module", "package", "io", "os", "debug",
        "collectgarbage", "newproxy", nullptr
    };
    for (int i = 0; dangerousGlobals[i] != nullptr; ++i) {
        lua_pushnil(L);
        lua_setglobal(L, dangerousGlobals[i]);
    }

    lua_sethook(L, instructionLimitHook, LUA_MASKCOUNT, 250000);

    if (luaL_dostring(L, luaCode.toUtf8().constData()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        qWarning() << "Lua theme execution error:" << err;
        lua_close(L);
        return {};
    }

    ThemeDefinition theme = extractFromTable(L);
    lua_close(L);
    return theme;
}

ThemeDefinition Parser::parseFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return parseScript(QString::fromUtf8(file.readAll()));
}

} // namespace Theme