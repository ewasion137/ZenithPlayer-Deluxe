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
    if (path.isEmpty()) return;

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
            // Track already exists: keep its chronological position
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