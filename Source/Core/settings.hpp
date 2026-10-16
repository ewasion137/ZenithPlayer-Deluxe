#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

namespace Core {

class Settings : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList webTracks READ webTracks NOTIFY webTracksChanged)

public:
    explicit Settings(QObject* parent = nullptr);

    QVariantList webTracks() const;

    Q_INVOKABLE QVariantMap getTrackSettings(const QString& path);
    Q_INVOKABLE void saveTrackSettings(const QString& path, float volume, float speed, const QVariantList& eq);
    Q_INVOKABLE void resetTrackSettings(const QString& path);

    Q_INVOKABLE void addWebTrack(const QString& id, const QString& url, const QString& name, const QString& artist, const QString& picture);
    Q_INVOKABLE void removeWebTrack(const QString& url);

signals:
    void settingsSaved();
    void webTracksChanged();

private:
    void load();
    void persist();
    void loadWebTracks();
    void persistWebTracks();

    QString settingsFilePath() const;
    QString webTracksFilePath() const;

    QJsonObject m_data;
    QJsonArray m_webTracks;
};

} // namespace Core