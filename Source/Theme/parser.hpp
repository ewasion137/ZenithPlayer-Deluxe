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
    QString target;     // e.g. "title", "play_button", "eq"
    QString property;   // e.g. "color", "scale", "opacity"
    QVariant fromValue;
    QVariant toValue;
    int duration{1000};
    bool loop{false};
    QString trigger{"none"}; // "none", "hover", "playing"
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
    static ThemeDefinition parse(const QString& content);
    static ThemeDefinition parseFile(const QString& filePath);
};

} // namespace Theme