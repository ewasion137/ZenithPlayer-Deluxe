#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QJsonObject>

namespace Core {

class Settings : public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject* parent = nullptr);

    Q_INVOKABLE QVariantMap getTrackSettings(const QString& path);
    Q_INVOKABLE void saveTrackSettings(const QString& path, float volume, float speed, const QVariantList& eq);
    Q_INVOKABLE void resetTrackSettings(const QString& path);

signals:
    void settingsSaved();

private:
    void load();
    void persist();
    QString settingsFilePath() const;

    QJsonObject m_data;
};

} // namespace Core