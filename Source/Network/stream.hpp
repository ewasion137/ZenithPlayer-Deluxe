#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QProcess>
#include <QString>

namespace Network {

class StreamManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY busyChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    explicit StreamManager(QObject* parent = nullptr);
    ~StreamManager() override;

    bool isBusy() const;
    QString statusText() const;

    Q_INVOKABLE void resolveAndDownload(const QString& queryOrUrl);
    Q_INVOKABLE void purgeCache();

signals:
    void busyChanged();
    void statusTextChanged();
    void streamReady(const QString& filePath, const QString& title, const QString& artist, const QString& coverPath, const QString& originalUrl, const QString& remoteCoverUrl);
    void streamError(const QString& message);

private:
    void setStatus(const QString& text, bool busy);
    QString ensureYtDlpBinary();
    void processDownload(const QString& targetUrl, const QString& customTitle = "", const QString& customArtist = "", const QString& customCover = "");
    void fetchSpotifyMetadata(const QString& url);
    void downloadCoverAndFinish(const QString& coverUrl, const QString& audioPath, const QString& title, const QString& artist, const QString& originalUrl, const QString& trackId);
    QString cacheDirectory() const;

    bool m_isBusy{false};
    QString m_statusText{"Ready"};
    QNetworkAccessManager m_netManager;
    QProcess m_process;
};

} // namespace Network