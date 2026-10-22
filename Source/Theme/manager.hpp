#pragma once

#include "parser.hpp"

#include <QColor>
#include <QNetworkAccessManager>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

namespace Core {
class Settings;
}

namespace Theme {

class Manager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableThemes READ availableThemes NOTIFY availableThemesChanged)
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE applyTheme NOTIFY currentThemeChanged)

    Q_PROPERTY(QColor bgMain READ bgMain NOTIFY themeUpdated)
    Q_PROPERTY(QColor panelBg READ panelBg NOTIFY themeUpdated)
    Q_PROPERTY(QColor surface READ surface NOTIFY themeUpdated)
    Q_PROPERTY(QColor accent READ accent NOTIFY themeUpdated)
    Q_PROPERTY(QColor accentSecondary READ accentSecondary NOTIFY themeUpdated)
    Q_PROPERTY(QColor accentGlow READ accentGlow NOTIFY themeUpdated)
    Q_PROPERTY(QColor textMain READ textMain NOTIFY themeUpdated)
    Q_PROPERTY(QColor textDim READ textDim NOTIFY themeUpdated)
    Q_PROPERTY(QColor border READ border NOTIFY themeUpdated)

    Q_PROPERTY(QString bgImageUrl READ bgImageUrl NOTIFY themeUpdated)
    Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY themeUpdated)
    Q_PROPERTY(QVariantMap textOverrides READ textOverrides NOTIFY themeUpdated)
    Q_PROPERTY(QVariantMap geometryOverrides READ geometryOverrides NOTIFY themeUpdated)
    Q_PROPERTY(QVariantList animations READ animations NOTIFY themeUpdated)
    Q_PROPERTY(QVariantMap effects READ effects NOTIFY themeUpdated)
    Q_PROPERTY(QVariantMap borders READ borders NOTIFY themeUpdated)

public:
    explicit Manager(Core::Settings* settings, QObject* parent = nullptr);

    QStringList availableThemes() const;
    QString currentTheme() const;

    QColor bgMain() const;
    QColor panelBg() const;
    QColor surface() const;
    QColor accent() const;
    QColor accentSecondary() const;
    QColor accentGlow() const;
    QColor textMain() const;
    QColor textDim() const;
    QColor border() const;

    QString bgImageUrl() const;
    QString fontFamily() const;
    QVariantMap textOverrides() const;
    QVariantMap geometryOverrides() const;
    QVariantList animations() const;

    Q_INVOKABLE void openThemesFolder();
    Q_INVOKABLE void refreshThemes();
    Q_INVOKABLE void applyTheme(const QString& name);
    Q_INVOKABLE QVariantMap getAnimation(const QString& target, const QString& property) const;
    Q_INVOKABLE QVariantList getAnimationsForTarget(const QString& target) const;
    QVariantMap effects() const { return m_activeDef.effects; }
    QVariantMap borders() const { return m_activeDef.borders; }

signals:
    void availableThemesChanged();
    void currentThemeChanged();
    void themeUpdated();

private:
    void ensureThemesDirectory();
    void createStarterThemes();
    void loadBgImage(const QString& pathOrUrl);
    QString themesDirectory() const;

    Core::Settings* m_settingsStore{nullptr};
    QStringList m_availableThemes;
    QString m_currentTheme{"Default"};
    ThemeDefinition m_activeDef;
    QString m_localBgPath{""};

    QNetworkAccessManager m_netManager;
};

} // namespace Theme