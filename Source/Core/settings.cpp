#include "settings.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
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

    load();
}

QString Settings::settingsFilePath() const {
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return appData + "/zenith-settings.json";
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

QVariantMap Settings::getTrackSettings(const QString& path) {
    if (path.isEmpty()) {
        return {};
    }

    if (m_data.contains(path)) {
        return m_data.value(path).toObject().toVariantMap();
    }
    return {};
}

void Settings::saveTrackSettings(const QString& path, float volume, float speed, const QVariantList& eq) {
    if (path.isEmpty()) {
        return;
    }

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
    if (path.isEmpty()) {
        return;
    }

    m_data.remove(path);
    persist();
}

} // namespace Core