#include "manager.hpp"
#include "security.hpp"
#include "Core/settings.hpp"

#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUrl>

namespace Theme {

Manager::Manager(Core::Settings* settings, QObject* parent)
    : QObject(parent), m_settingsStore(settings) {
    ensureThemesDirectory();
    createStarterThemes();
    refreshThemes();

    // Default palette
    m_activeDef.colors["bg_main"] = QColor("#08090d");
    m_activeDef.colors["panel_bg"] = QColor("#10121a");
    m_activeDef.colors["surface"] = QColor("#181a26");
    m_activeDef.colors["accent"] = QColor("#8b5cf6");
    m_activeDef.colors["accent_secondary"] = QColor("#38bdf8");
    m_activeDef.colors["text_main"] = QColor("#f8fafc");
    m_activeDef.colors["text_dim"] = QColor("#717b94");
    m_activeDef.colors["border"] = QColor("#1e2235");
    m_activeDef.colors["accent_glow"] = QColor("#3b2d66");

    if (m_availableThemes.contains("Default.zss")) {
        applyTheme("Default.zss");
    }
}

QString Manager::themesDirectory() const {
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appData + "/Themes";
}

void Manager::ensureThemesDirectory() {
    QDir().mkpath(themesDirectory());
}

void Manager::createStarterThemes() {
    const QString defaultPath = themesDirectory() + "/Default.zss";
    if (!QFile::exists(defaultPath)) {
        QFile file(defaultPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(
                "#^ Author: Zenith\n"
                "#^ ThemeName: Default\n"
                "#^ Version: 1.0\n"
                "#^ Description: Factory dark theme\n\n"
                "colors {\n"
                "    bg_main: #08090d;\n"
                "    panel_bg: #10121a;\n"
                "    surface: #181a26;\n"
                "    accent: #8b5cf6;\n"
                "    accent_secondary: #38bdf8;\n"
                "    text_main: #f8fafc;\n"
                "    text_dim: #717b94;\n"
                "    border: #1e2235;\n"
                "}\n"
            );
        }
    }

    const QString cyberPath = themesDirectory() + "/Cyberpunk_Neon.zss";
    if (!QFile::exists(cyberPath)) {
        QFile file(cyberPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(
                "#^ Author: Zenith\n"
                "#^ ThemeName: Cyberpunk Neon\n"
                "#^ Version: 1.0\n"
                "#^ Description: Electric high-contrast synth aesthetic\n\n"
                "colors {\n"
                "    bg_main: #040209;\n"
                "    panel_bg: #0d061a;\n"
                "    surface: #190b33;\n"
                "    accent: #00f0ff;\n"
                "    accent_secondary: #ff0055;\n"
                "    text_main: #ffffff;\n"
                "    text_dim: #9d8ec2;\n"
                "    border: #2e125c;\n"
                "}\n\n"
                "text {\n"
                "    window_title: \"CYBERPUNK // AUDIO TERMINAL\";\n"
                "    library_title: \"NEO ARCHIVE\";\n"
                "}\n\n"
                "animation neon_title_glow {\n"
                "    target: \"title\";\n"
                "    property: \"color\";\n"
                "    from: \"#00f0ff\";\n"
                "    to: \"#ff0055\";\n"
                "    duration: 2500;\n"
                "    loop: infinite;\n"
                "}\n"
            );
        }
    }

    const QString linearPath = themesDirectory() + "/Linear_Deluxe.zss";
    if (!QFile::exists(linearPath)) {
        QFile file(linearPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(
                "#^ Author: Zenith Deluxe\n"
                "#^ ThemeName: Linear Deluxe\n"
                "#^ Version: 2.0\n"
                "#^ Description: Raycast & Linear inspired hyper-fluid aesthetic with physics-based spring animations\n\n"
                "@font \"Segoe UI\"\n\n"
                "colors {\n"
                "    bg_main: #090a0f;\n"
                "    panel_bg: #11131c;\n"
                "    surface: #181b28;\n"
                "    accent: #8b5cf6;\n"
                "    accent_secondary: #38bdf8;\n"
                "    accent_glow: #6366f1;\n"
                "    text_main: #f8fafc;\n"
                "    text_dim: #79829c;\n"
                "    border: #222638;\n"
                "}\n\n"
                "text {\n"
                "    window_title: \"ZENITH // LINEAR DELUXE\";\n"
                "}\n\n"
                "animation title_chroma {\n"
                "    target: \"title\";\n"
                "    property: \"color\";\n"
                "    from: \"#8b5cf6\";\n"
                "    to: \"#38bdf8\";\n"
                "    duration: 3500;\n"
                "    loop: infinite;\n"
                "    easing: in-out-quad;\n"
                "}\n\n"
                "animation vinyl_cover_spin {\n"
                "    target: \"cover\";\n"
                "    property: \"rotation\";\n"
                "    from: 0;\n"
                "    to: 360;\n"
                "    duration: 6000;\n"
                "    loop: infinite;\n"
                "    direction: forward;\n"
                "    trigger: playing;\n"
                "    easing: linear;\n"
                "}\n\n"
                "animation play_btn_spring {\n"
                "    target: \"play_button\";\n"
                "    property: \"scale\";\n"
                "    from: 1.0;\n"
                "    to: 1.15;\n"
                "    duration: 180;\n"
                "    trigger: hover;\n"
                "    easing: spring;\n"
                "}\n"
            );
        }
    }

    const QString hyprPath = themesDirectory() + "/Hyprland_Niri.zss";
    if (!QFile::exists(hyprPath)) {
        QFile file(hyprPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(
                "#^ Author: Zenith Deluxe\n"
                "#^ ThemeName: Hyprland Niri\n"
                "#^ Version: 3.0\n"
                "#^ Description: Hyper-kinetic Wayland compositor aesthetic with spring curves\n\n"
                "@font \"Consolas\"\n\n"
                "colors {\n"
                "    bg_main: #06070a;\n"
                "    panel_bg: #0c0e14;\n"
                "    surface: #141722;\n"
                "    accent: #00f0ff;\n"
                "    accent_secondary: #ff007f;\n"
                "    accent_glow: #00f0ff;\n"
                "    text_main: #f0f6fc;\n"
                "    text_dim: #768390;\n"
                "    border: #23293e;\n"
                "}\n\n"
                "text {\n"
                "    window_title: \"ZENITH // HYPRLAND [NIRI-WORKSPACE-01]\";\n"
                "}\n\n"
                "animation title_chroma {\n"
                "    target: \"title\";\n"
                "    property: \"color\";\n"
                "    from: \"#00f0ff\";\n"
                "    to: \"#ff007f\";\n"
                "    duration: 3000;\n"
                "    loop: infinite;\n"
                "    easing: in-out-quad;\n"
                "}\n\n"
                "animation play_pop {\n"
                "    target: \"play_button\";\n"
                "    property: \"scale\";\n"
                "    from: 1.0;\n"
                "    to: 1.20;\n"
                "    duration: 160;\n"
                "    trigger: hover;\n"
                "    easing: spring;\n"
                "}\n\n"
                "animation vinyl_spin {\n"
                "    target: \"cover\";\n"
                "    property: \"rotation\";\n"
                "    from: 0;\n"
                "    to: 360;\n"
                "    duration: 4500;\n"
                "    loop: infinite;\n"
                "    direction: forward;\n"
                "    trigger: playing;\n"
                "    easing: linear;\n"
                "}\n"
            );
        }
    }
}

QStringList Manager::availableThemes() const {
    return m_availableThemes;
}

QString Manager::currentTheme() const {
    return m_currentTheme;
}

QColor Manager::bgMain() const { return m_activeDef.colors.value("bg_main", QColor("#08090d")); }
QColor Manager::panelBg() const { return m_activeDef.colors.value("panel_bg", QColor("#10121a")); }
QColor Manager::surface() const { return m_activeDef.colors.value("surface", QColor("#181a26")); }
QColor Manager::accent() const { return m_activeDef.colors.value("accent", QColor("#8b5cf6")); }
QColor Manager::accentSecondary() const { return m_activeDef.colors.value("accent_secondary", QColor("#38bdf8")); }
QColor Manager::textMain() const { return m_activeDef.colors.value("text_main", QColor("#f8fafc")); }
QColor Manager::textDim() const { return m_activeDef.colors.value("text_dim", QColor("#717b94")); }
QColor Manager::border() const { return m_activeDef.colors.value("border", QColor("#1e2235")); }
QColor Manager::accentGlow() const {
    if (m_activeDef.colors.contains("accent_glow")) {
        return m_activeDef.colors.value("accent_glow");
    }
    QColor glow = accent();
    glow.setAlpha(60);
    return glow;
}

QString Manager::bgImageUrl() const { return m_localBgPath; }
QString Manager::fontFamily() const { return m_activeDef.fontFamily; }

QVariantMap Manager::textOverrides() const {
    QVariantMap map;
    for (auto it = m_activeDef.textOverrides.begin(); it != m_activeDef.textOverrides.end(); ++it) {
        map[it.key()] = it.value();
    }
    return map;
}

QVariantMap Manager::geometryOverrides() const {
    QVariantMap map;
    for (auto it = m_activeDef.geometry.begin(); it != m_activeDef.geometry.end(); ++it) {
        map[it.key()] = it.value();
    }
    return map;
}

QVariantList Manager::animations() const {
    QVariantList list;
    for (const auto& a : m_activeDef.animations) {
        QVariantMap obj;
        obj["name"] = a.name;
        obj["target"] = a.target;
        obj["property"] = a.property;
        obj["from"] = a.fromValue;
        obj["to"] = a.toValue;
        obj["duration"] = a.duration;
        obj["loop"] = a.loop;
        obj["trigger"] = a.trigger;
        obj["easing"] = a.easing;
        obj["direction"] = a.direction;
        obj["delay"] = a.delay;
        list.append(obj);
    }
    return list;
}

void Manager::openThemesFolder() {
    ensureThemesDirectory();
    QDesktopServices::openUrl(QUrl::fromLocalFile(themesDirectory()));
}

void Manager::refreshThemes() {
    ensureThemesDirectory();
    QDir dir(themesDirectory());
    m_availableThemes = dir.entryList(QStringList() << "*.zss", QDir::Files);
    emit availableThemesChanged();
}

void Manager::applyTheme(const QString& name) {
    const QString filePath = themesDirectory() + "/" + name;
    if (!QFile::exists(filePath)) return;

    m_activeDef = Parser::parseFile(filePath);
    m_currentTheme = name;
    m_localBgPath = "";

    // Применение и регистрация шрифтов (включая локальные .ttf / .otf файлы)
    if (!m_activeDef.fontFamily.isEmpty()) {
        QString family = m_activeDef.fontFamily;
        if (family.endsWith(".ttf", Qt::CaseInsensitive) || family.endsWith(".otf", Qt::CaseInsensitive)) {
            QString fontFile = family;
            if (!QFileInfo(fontFile).isAbsolute()) {
                fontFile = themesDirectory() + "/" + family;
            }
            if (QFile::exists(fontFile)) {
                const int id = QFontDatabase::addApplicationFont(fontFile);
                if (id != -1) {
                    const QStringList families = QFontDatabase::applicationFontFamilies(id);
                    if (!families.isEmpty()) {
                        family = families.first();
                        m_activeDef.fontFamily = family;
                    }
                }
            }
        }
        QFont f(family);
        QGuiApplication::setFont(f);
    }

    if (!m_activeDef.bgImage.isEmpty()) {
        loadBgImage(m_activeDef.bgImage);
    }

    emit currentThemeChanged();
    emit themeUpdated();
}

void Manager::loadBgImage(const QString& pathOrUrl) {
    if (!Security::isPathSafe(pathOrUrl)) return;

    if (pathOrUrl.startsWith("http://") || pathOrUrl.startsWith("https://")) {
        QNetworkRequest req{QUrl(pathOrUrl)};
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply* reply = m_netManager.get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            reply->deleteLater();
            if (reply->error() == QNetworkReply::NoError) {
                const QByteArray bytes = reply->readAll();
                if (Security::validateImageBytes(bytes)) {
                    const QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/theme_bg.jpg";
                    QFile file(cachePath);
                    if (file.open(QIODevice::WriteOnly)) {
                        file.write(bytes);
                        file.close();
                        m_localBgPath = QUrl::fromLocalFile(cachePath).toString();
                        emit themeUpdated();
                    }
                }
            }
        });
    } else {
        QFile file(pathOrUrl);
        if (file.open(QIODevice::ReadOnly)) {
            const QByteArray bytes = file.read(1024);
            if (Security::validateImageBytes(bytes)) {
                m_localBgPath = QUrl::fromLocalFile(pathOrUrl).toString();
                emit themeUpdated();
            }
        }
    }
}

QVariantMap Manager::getAnimation(const QString& target, const QString& property) const {
    for (const auto& a : m_activeDef.animations) {
        if (a.target.compare(target, Qt::CaseInsensitive) == 0 &&
            (property.isEmpty() || a.property.compare(property, Qt::CaseInsensitive) == 0)) {
            QVariantMap obj;
            obj["name"] = a.name;
            obj["target"] = a.target;
            obj["property"] = a.property;
            obj["from"] = a.fromValue;
            obj["to"] = a.toValue;
            obj["duration"] = a.duration;
            obj["loop"] = a.loop;
            obj["trigger"] = a.trigger;
            obj["easing"] = a.easing;
            obj["direction"] = a.direction;
            obj["delay"] = a.delay;
            return obj;
        }
    }
    return {};
}

QVariantList Manager::getAnimationsForTarget(const QString& target) const {
    QVariantList list;
    for (const auto& a : m_activeDef.animations) {
        if (a.target.compare(target, Qt::CaseInsensitive) == 0) {
            QVariantMap obj;
            obj["name"] = a.name;
            obj["target"] = a.target;
            obj["property"] = a.property;
            obj["from"] = a.fromValue;
            obj["to"] = a.toValue;
            obj["duration"] = a.duration;
            obj["loop"] = a.loop;
            obj["trigger"] = a.trigger;
            obj["easing"] = a.easing;
            obj["direction"] = a.direction;
            obj["delay"] = a.delay;
            list.append(obj);
        }
    }
    return list;
}

} // namespace Theme