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
    QString target;
    QString property;
    QVariant fromValue;
    QVariant toValue;
    int duration{250};
    bool loop{false};
    QString trigger{"none"};
    QString easing{"spring"};
    QString direction{"alternate"};
    int delay{0};
    float overshoot{1.45f};
    QVariantList bezier;
};

struct ThemeDefinition {
    ThemeHeader header;
    QString bgImage{""};
    QString fontFamily{""};

    QMap<QString, QColor> colors;
    QMap<QString, double> geometry;
    QMap<QString, QString> textOverrides;
    QVariantMap effects;
    QVariantMap borders;
    std::vector<ThemeAnimation> animations;
};

class Parser {
public:
    static ThemeDefinition parseFile(const QString& filePath);
    static ThemeDefinition parseScript(const QString& luaCode);
};

} // namespace Theme