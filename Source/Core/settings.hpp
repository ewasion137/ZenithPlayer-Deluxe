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
    Q_PROPERTY(bool flStudioMode READ flStudioMode WRITE setFlStudioMode NOTIFY appSettingsChanged)
    Q_PROPERTY(bool smoothFade READ smoothFade WRITE setSmoothFade NOTIFY appSettingsChanged)
    Q_PROPERTY(bool declickSeek READ declickSeek WRITE setDeclickSeek NOTIFY appSettingsChanged)
    Q_PROPERTY(bool autoAdvance READ autoAdvance WRITE setAutoAdvance NOTIFY appSettingsChanged)
    Q_PROPERTY(bool showSpectrogram READ showSpectrogram WRITE setShowSpectrogram NOTIFY appSettingsChanged)
    Q_PROPERTY(bool showWaveform READ showWaveform WRITE setShowWaveform NOTIFY appSettingsChanged)
    Q_PROPERTY(bool showCoverGlow READ showCoverGlow WRITE setShowCoverGlow NOTIFY appSettingsChanged)
    Q_PROPERTY(bool enableDiscordRpc READ enableDiscordRpc WRITE setEnableDiscordRpc NOTIFY appSettingsChanged)
    Q_PROPERTY(bool globalHotkeys READ globalHotkeys WRITE setGlobalHotkeys NOTIFY appSettingsChanged)
    Q_PROPERTY(bool autoSaveTracks READ autoSaveTracks WRITE setAutoSaveTracks NOTIFY appSettingsChanged)
    Q_PROPERTY(int spectrogramFps READ spectrogramFps WRITE setSpectrogramFps NOTIFY appSettingsChanged)


public:
    explicit Settings(QObject* parent = nullptr);

    QVariantList webTracks() const;

    bool flStudioMode() const;
    void setFlStudioMode(bool val);

    bool smoothFade() const;
    void setSmoothFade(bool val);

    bool declickSeek() const;
    void setDeclickSeek(bool val);

    bool autoAdvance() const;
    void setAutoAdvance(bool val);

    bool showSpectrogram() const;
    void setShowSpectrogram(bool val);

    bool showWaveform() const;
    void setShowWaveform(bool val);

    bool showCoverGlow() const;
    void setShowCoverGlow(bool val);

    bool enableDiscordRpc() const;
    void setEnableDiscordRpc(bool val);

    bool globalHotkeys() const;
    void setGlobalHotkeys(bool val);

    bool autoSaveTracks() const;
    void setAutoSaveTracks(bool val);
    int spectrogramFps() const;
    void setSpectrogramFps(int fps);

    Q_INVOKABLE void clearAllTrackSettings();

    Q_INVOKABLE QVariantMap getTrackSettings(const QString& path);
    Q_INVOKABLE void saveTrackSettings(const QString& path, float volume, float speed, const QVariantList& eq);
    Q_INVOKABLE void resetTrackSettings(const QString& path);

    Q_INVOKABLE void addWebTrack(const QString& id, const QString& url, const QString& name, const QString& artist, const QString& picture);
    Q_INVOKABLE void removeWebTrack(const QString& url);
    Q_INVOKABLE QString getDiscordClientId();
    Q_INVOKABLE void setDiscordClientId(const QString& id);

signals:
    void settingsSaved();
    void webTracksChanged();
    void appSettingsChanged();

private:
    void load();
    void persist();
    void loadWebTracks();
    void persistWebTracks();

    bool getBoolSetting(const QString& key, bool defaultVal) const;
    void setBoolSetting(const QString& key, bool val);

    QString settingsFilePath() const;
    QString webTracksFilePath() const;

    QJsonObject m_data;
    QJsonArray m_webTracks;
};

} // namespace Core