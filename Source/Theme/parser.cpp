#include "parser.hpp"
#include "security.hpp"

#include <QFile>
#include <QRegularExpression>
#include <QStringConverter>

namespace Theme {

namespace {

QString extractBlockContent(const QString& text, const QString& blockName) {
    const QRegularExpression regex(blockName + R"(\s*\{([^}]*)\})");
    const auto match = regex.match(text);
    return match.hasMatch() ? match.captured(1) : "";
}

} // namespace

ThemeDefinition Parser::parse(const QString& content) {
    ThemeDefinition theme;
    const QStringList lines = content.split('\n');

    // 1. Parse #^ Header and @ Directives
    for (const QString& rawLine : lines) {
        const QString line = rawLine.trimmed();

        if (line.startsWith("#^")) {
            const QString headerLine = line.mid(2).trimmed();
            const int colon = headerLine.indexOf(':');
            if (colon != -1) {
                const QString key = headerLine.left(colon).trimmed().toLower();
                const QString val = Security::sanitizeString(headerLine.mid(colon + 1));

                if (key == "author") theme.header.author = val;
                else if (key == "themename") theme.header.themeName = val;
                else if (key == "version") theme.header.version = val;
                else if (key == "description") theme.header.description = val;
            }
        } else if (line.startsWith("@bg")) {
            const QRegularExpression bgRegex(R"(@bg\s*["']([^"']+)["'])");
            const auto m = bgRegex.match(line);
            if (m.hasMatch()) {
                const QString rawUrl = m.captured(1).trimmed();
                if (Security::isPathSafe(rawUrl)) {
                    theme.bgImage = rawUrl;
                }
            }
        } else if (line.startsWith("@font")) {
            const QRegularExpression fontRegex(R"(@font\s*["']([^"']+)["'])");
            const auto m = fontRegex.match(line);
            if (m.hasMatch()) {
                theme.fontFamily = Security::sanitizeString(m.captured(1));
            }
        }
    }

    // 2. Parse colors { ... }
    const QString colorsBlock = extractBlockContent(content, "colors");
    if (!colorsBlock.isEmpty()) {
        const QStringList colorLines = colorsBlock.split(';');
        for (const QString& item : colorLines) {
            const int colon = item.indexOf(':');
            if (colon != -1) {
                const QString key = item.left(colon).trimmed().toLower();
                const QString val = item.mid(colon + 1).trimmed();
                if (QColor::isValidColorName(val)) {
                    theme.colors[key] = QColor(val);
                }
            }
        }
    }

    // 3. Parse geometry { ... }
    const QString geomBlock = extractBlockContent(content, "geometry");
    if (!geomBlock.isEmpty()) {
        const QStringList geomLines = geomBlock.split(';');
        for (const QString& item : geomLines) {
            const int colon = item.indexOf(':');
            if (colon != -1) {
                const QString key = item.left(colon).trimmed().toLower();
                bool ok = false;
                const double val = item.mid(colon + 1).trimmed().toDouble(&ok);
                if (ok) {
                    theme.geometry[key] = val;
                }
            }
        }
    }

    // 4. Parse text { ... } (Only static labels allowed)
    const QString textBlock = extractBlockContent(content, "text");
    if (!textBlock.isEmpty()) {
        const QStringList textLines = textBlock.split(';');
        for (const QString& item : textLines) {
            const int colon = item.indexOf(':');
            if (colon != -1) {
                const QString key = item.left(colon).trimmed().toLower();
                QString val = item.mid(colon + 1).trimmed();
                if (val.startsWith('"') && val.endsWith('"') && val.length() >= 2) {
                    val = val.mid(1, val.length() - 2);
                }
                theme.textOverrides[key] = Security::sanitizeString(val);
            }
        }
    }

    // 5. Parse animation <name> { ... }
    const QRegularExpression animRegex(R"(animation\s+([a-zA-Z0-9_-]+)\s*\{([^}]*)\})");
    auto animIt = animRegex.globalMatch(content);
    while (animIt.hasNext()) {
        const auto m = animIt.next();
        ThemeAnimation anim;
        anim.name = m.captured(1);

        const QString body = m.captured(2);
        const QStringList props = body.split(';');
        for (const QString& p : props) {
            const int colon = p.indexOf(':');
            if (colon != -1) {
                const QString k = p.left(colon).trimmed().toLower();
                const QString v = p.mid(colon + 1).trimmed().remove('"');

                if (k == "target") anim.target = v;
                else if (k == "property") anim.property = v;
                else if (k == "from") anim.fromValue = v;
                else if (k == "to") anim.toValue = v;
                else if (k == "duration") anim.duration = v.toInt();
                else if (k == "loop") anim.loop = (v.toLower() == "infinite" || v.toLower() == "true");
                else if (k == "trigger") anim.trigger = v.toLower();
            }
        }

        if (!anim.target.isEmpty() && !anim.property.isEmpty()) {
            theme.animations.push_back(anim);
        }
    }

    return theme;
}

ThemeDefinition Parser::parseFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return parse(QString::fromUtf8(file.readAll()));
}

} // namespace Theme