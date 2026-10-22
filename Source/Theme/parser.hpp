#pragma once

#include <QColor>
#include <QMap>
#include <QString>
#include <QVariant>
#include <vector>

namespace Theme {

struct ThemeHeader {
    QString author{"Unknown"};
    QString themeName{"Default"};
    QString version{"1.0"};
    QString description{""};
};

struct ThemeAnimation {
    QString name;
    QString target;     // e.g. "title", "play_button", "cover"
    QString property;   // e.g. "color", "scale", "rotation", "opacity", "y", "x"
    QVariant fromValue;
    QVariant toValue;
    int duration{1000};
    bool loop{false};
    QString trigger{"none"}; // "none", "hover", "playing"
    QString easing{"spring"};
    QString direction{"alternate"}; // "alternate", "forward", "reverse"
    int delay{0};
};

struct ThemeDefinition {
    ThemeHeader header;
    QString bgImage{""};
    QString fontFamily{""};

    QMap<QString, QColor> colors;
    QMap<QString, double> geometry;
    QMap<QString, QString> textOverrides;
    std::vector<ThemeAnimation> animations;
};

class Parser {
public:
    static ThemeDefinition parseFile(const QString& filePath);
    static ThemeDefinition parseScript(const QString& luaCode);
};

} // namespace Theme