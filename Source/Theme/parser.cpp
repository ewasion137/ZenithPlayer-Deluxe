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

ThemeDefinition extractFromTable(lua_State* L) {
    ThemeDefinition theme;
    if (!lua_istable(L, -1)) return theme;

    // 1. Meta / Header
    lua_pushstring(L, "meta");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        theme.header.themeName = Security::sanitizeString(getStringField(L, "name", "Default"));
        theme.header.author = Security::sanitizeString(getStringField(L, "author", "Unknown"));
        theme.header.version = Security::sanitizeString(getStringField(L, "version", "1.0"));
        theme.header.description = Security::sanitizeString(getStringField(L, "description", ""));
    }
    lua_pop(L, 1);

    if (theme.header.themeName == "Default") {
        QString n = getStringField(L, "name", "");
        if (!n.isEmpty()) theme.header.themeName = Security::sanitizeString(n);
    }

    // 2. Font
    QString font = getStringField(L, "font", "");
    if (font.isEmpty()) font = getStringField(L, "font_family", "");
    theme.fontFamily = Security::sanitizeString(font);

    // 3. Background Image
    QString bg = getStringField(L, "bg_image", "");
    if (bg.isEmpty()) bg = getStringField(L, "bg", "");
    if (Security::isPathSafe(bg)) {
        theme.bgImage = bg;
    }

    // 4. Colors
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

    // 5. Geometry
    lua_pushstring(L, "geometry");
    if (lua_gettable(L, -2) == LUA_TTABLE) {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            if (lua_isstring(L, -2) && lua_isnumber(L, -1)) {
                QString key = QString::fromUtf8(lua_tostring(L, -2)).trimmed().toLower();
                double val = lua_tonumber(L, -1);
                theme.geometry[key] = val;
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    // 6. Text Overrides
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

    // 7. Animations
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
                a.duration = static_cast<int>(getNumberField(L, "duration", 1000));
                a.loop = getBoolField(L, "loop", false);
                a.trigger = getStringField(L, "trigger", "none").toLower();
                a.easing = getStringField(L, "easing", "spring").toLower();
                a.direction = getStringField(L, "direction", "alternate").toLower();
                a.delay = static_cast<int>(getNumberField(L, "delay", 0));

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

ThemeDefinition Parser::parseFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open Lua theme file:" << filePath;
        return {};
    }
    const QString script = QString::fromUtf8(file.readAll());
    return parseScript(script);
}
// Добавьте эту функцию-хук перед parseScript
static void instructionLimitHook(lua_State* L, lua_Debug* ar) {
    Q_UNUSED(ar);
    luaL_error(L, "Security violation: Lua script instruction limit exceeded (freeze/loop detected)");
}

ThemeDefinition Parser::parseScript(const QString& luaCode) {
    lua_State* L = luaL_newstate();
    if (!L) return {};

    // 1. Подключаем строго изолированные безопасные библиотеки
    luaL_requiref(L, "_G", luaopen_base, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
    lua_pop(L, 1);
    luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
    lua_pop(L, 1);

    // 2. В базовой библиотеке все еще есть функции для выполнения файлов/кода. Вычищаем их:
    const char* dangerousGlobals[] = {
        "dofile", "loadfile", "load", "loadstring",
        "require", "module", "package", "io", "os", "debug",
        "collectgarbage", "newproxy", nullptr
    };
    for (int i = 0; dangerousGlobals[i] != nullptr; ++i) {
        lua_pushnil(L);
        lua_setglobal(L, dangerousGlobals[i]);
    }

    // 3. Защита от while true do end (максимум 250 000 виртуальных инструкций на тему)
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

} // namespace Theme