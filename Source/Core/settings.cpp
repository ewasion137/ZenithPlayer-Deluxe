#include "settings.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace Core {

Settings::Settings(QObject* parent)
    : QObject(parent) {
    const QString path = settingsFilePath();
    const QFileInfo info(path);
    QDir().mkpath(info.dir().absolutePath());

    if (!QFile::exists(path)) {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("{}");
        }
    }

    const QString webPath = webTracksFilePath();
    if (!QFile::exists(webPath)) {
        QFile file(webPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write("[]");
        }
    }

    load();
    loadWebTracks();
}

QString Settings::settingsFilePath() const {
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appData + "/zenith-settings.json";
}

QString Settings::webTracksFilePath() const {
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appData + "/zenith-web-tracks.json";
}

void Settings::load() {
    QFile file(settingsFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject()) {
            m_data = doc.object();
        }
    }
}

void Settings::persist() {
    QFile file(settingsFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(m_data).toJson(QJsonDocument::Indented));
    }
}

void Settings::loadWebTracks() {
    QFile file(webTracksFilePath());
    if (file.open(QIODevice::ReadOnly)) {
        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) {
            m_webTracks = doc.array();
        }
    }
}

void Settings::persistWebTracks() {
    QFile file(webTracksFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(m_webTracks).toJson(QJsonDocument::Indented));
    }
}

bool Settings::getBoolSetting(const QString& key, bool defaultVal) const {
    const QJsonObject config = m_data.value("app_config").toObject();
    if (config.contains(key)) {
        return config.value(key).toBool(defaultVal);
    }
    return defaultVal;
}

void Settings::setBoolSetting(const QString& key, bool val) {
    QJsonObject config = m_data.value("app_config").toObject();
    if (!config.contains(key) || config.value(key).toBool() != val) {
        config[key] = val;
        m_data["app_config"] = config;
        persist();
        emit appSettingsChanged();
    }
}

int Settings::spectrogramFps() const {
    const QJsonObject config = m_data.value("app_config").toObject();
    return config.value("spectrogram_fps").toInt(60);
}

void Settings::setSpectrogramFps(int fps) {
    QJsonObject config = m_data.value("app_config").toObject();
    if (config.value("spectrogram_fps").toInt() != fps) {
        config["spectrogram_fps"] = fps;
        m_data["app_config"] = config;
        persist();
        emit appSettingsChanged();
    }
}

void Settings::clearAllTrackSettings() {
    QJsonObject cleaned;
    if (m_data.contains("app_config")) {
        cleaned["app_config"] = m_data["app_config"];
    }
    if (m_data.contains("discord_client_id")) {
        cleaned["discord_client_id"] = m_data["discord_client_id"];
    }
    m_data = cleaned;
    persist();
    emit settingsSaved();
}

bool Settings::flStudioMode() const { return getBoolSetting("fl_studio_mode", true); }
void Settings::setFlStudioMode(bool val) { setBoolSetting("fl_studio_mode", val); }

bool Settings::smoothFade() const { return getBoolSetting("smooth_fade", true); }
void Settings::setSmoothFade(bool val) { setBoolSetting("smooth_fade", val); }

bool Settings::declickSeek() const { return getBoolSetting("declick_seek", true); }
void Settings::setDeclickSeek(bool val) { setBoolSetting("declick_seek", val); }

bool Settings::autoAdvance() const { return getBoolSetting("auto_advance", true); }
void Settings::setAutoAdvance(bool val) { setBoolSetting("auto_advance", val); }

bool Settings::showSpectrogram() const { return getBoolSetting("show_spectrogram", true); }
void Settings::setShowSpectrogram(bool val) { setBoolSetting("show_spectrogram", val); }

bool Settings::showWaveform() const { return getBoolSetting("show_waveform", true); }
void Settings::setShowWaveform(bool val) { setBoolSetting("show_waveform", val); }

bool Settings::showCoverGlow() const { return getBoolSetting("show_cover_glow", true); }
void Settings::setShowCoverGlow(bool val) { setBoolSetting("show_cover_glow", val); }

bool Settings::enableDiscordRpc() const { return getBoolSetting("enable_discord_rpc", true); }
void Settings::setEnableDiscordRpc(bool val) { setBoolSetting("enable_discord_rpc", val); }

bool Settings::globalHotkeys() const { return getBoolSetting("global_hotkeys", true); }
void Settings::setGlobalHotkeys(bool val) { setBoolSetting("global_hotkeys", val); }

bool Settings::autoSaveTracks() const { return getBoolSetting("auto_save_tracks", true); }
void Settings::setAutoSaveTracks(bool val) { setBoolSetting("auto_save_tracks", val); }

QVariantList Settings::webTracks() const {
    return m_webTracks.toVariantList();
}

QVariantMap Settings::getTrackSettings(const QString& path) {
    if (path.isEmpty()) return {};
    if (m_data.contains(path)) {
        return m_data.value(path).toObject().toVariantMap();
    }
    return {};
}

void Settings::saveTrackSettings(const QString& path, float volume, float speed, const QVariantList& eq) {
    if (path.isEmpty() || !autoSaveTracks()) return;

    QJsonObject trackObj;
    trackObj["volume"] = volume;
    trackObj["speed"] = speed;

    QJsonArray eqArr;
    for (const auto& val : eq) {
        eqArr.append(val.toDouble());
    }
    trackObj["eq"] = eqArr;

    m_data[path] = trackObj;
    persist();
    emit settingsSaved();
}

void Settings::resetTrackSettings(const QString& path) {
    if (path.isEmpty()) return;
    m_data.remove(path);
    persist();
}

QString Settings::getDiscordClientId() {
    return m_data.value("discord_client_id").toString();
}

void Settings::setDiscordClientId(const QString& id) {
    m_data["discord_client_id"] = id;
    persist();
}

void Settings::addWebTrack(const QString& id, const QString& url, const QString& name, const QString& artist, const QString& picture) {
    for (int i = 0; i < m_webTracks.size(); ++i) {
        if (m_webTracks[i].toObject()["url"].toString() == url) {
            return;
        }
    }

    QJsonObject obj;
    obj["id"] = id;
    obj["url"] = url;
    obj["name"] = name;
    obj["artist"] = artist;
    obj["picture"] = picture;

    m_webTracks.prepend(obj);
    persistWebTracks();
    emit webTracksChanged();
}

void Settings::removeWebTrack(const QString& url) {
    for (int i = 0; i < m_webTracks.size(); ++i) {
        if (m_webTracks[i].toObject()["url"].toString() == url) {
            m_webTracks.removeAt(i);
            persistWebTracks();
            emit webTracksChanged();
            return;
        }
    }
}

} // namespace Core