#include "stream.hpp"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QUrl>

namespace Network {

StreamManager::StreamManager(QObject* parent)
    : QObject(parent) {
    purgeCache();
}

StreamManager::~StreamManager() {
    if (m_process.state() == QProcess::Running) {
        m_process.kill();
        m_process.waitForFinished(1000);
    }
    purgeCache();
}

bool StreamManager::isBusy() const {
    return m_isBusy;
}

QString StreamManager::statusText() const {
    return m_statusText;
}

void StreamManager::setStatus(const QString& text, bool busy) {
    m_statusText = text;
    m_isBusy = busy;
    emit statusTextChanged();
    emit busyChanged();
}

QString StreamManager::cacheDirectory() const {
    const QString cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/WebStream";
    QDir().mkpath(cachePath);
    return cachePath;
}

void StreamManager::purgeCache() {
    QDir dir(cacheDirectory());
    dir.removeRecursively();
    QDir().mkpath(cacheDirectory());
}

QString StreamManager::ensureYtDlpBinary() {
    const QString systemYtDlp = QStandardPaths::findExecutable("yt-dlp");
    if (!systemYtDlp.isEmpty()) {
        return systemYtDlp;
    }

    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString localBin = appData + "/bin/yt-dlp.exe";

    if (QFile::exists(localBin)) {
        return localBin;
    }

    QDir().mkpath(appData + "/bin");
    return "";
}

void StreamManager::resolveAndDownload(const QString& queryOrUrl) {
    if (m_isBusy) {
        return;
    }

    const QString input = queryOrUrl.trimmed();
    if (input.isEmpty()) {
        return;
    }

    if (input.contains("spotify.com/track/")) {
        fetchSpotifyMetadata(input);
        return;
    }

    processDownload(input);
}

void StreamManager::fetchSpotifyMetadata(const QString& spotifyUrl) {
    setStatus("Connecting to Spotify API...", true);

    const QString apiUrl = QString("https://open.spotify.com/oembed?url=%1").arg(spotifyUrl);
    QNetworkRequest req;
    req.setUrl(QUrl(apiUrl));
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    QNetworkReply* reply = m_netManager.get(req);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            setStatus("Spotify track resolution failed", false);
            emit streamError("Could not resolve Spotify track");
            return;
        }

        const auto doc = QJsonDocument::fromJson(reply->readAll());
        const auto obj = doc.object();

        const QString title = obj.value("title").toString();
        const QString artist = obj.value("author_name").toString();
        const QString cover = obj.value("thumbnail_url").toString();

        const QString ytQuery = QString("%1 %2").arg(artist, title);
        processDownload(ytQuery, title, artist, cover);
    });
}

void StreamManager::processDownload(const QString& target, const QString& customTitle, const QString& customArtist, const QString& customCover) {
    const QString ytDlp = ensureYtDlpBinary();

    if (ytDlp.isEmpty()) {
        setStatus("Downloading official yt-dlp engine...", true);
        const QUrl downloadUrl("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe");
        QNetworkRequest req(downloadUrl);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply* reply = m_netManager.get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, target, customTitle, customArtist, customCover]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                setStatus("Failed to download yt-dlp binary", false);
                emit streamError("yt-dlp download failed");
                return;
            }

            const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            const QString localBin = appData + "/bin/yt-dlp.exe";
            QFile file(localBin);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(reply->readAll());
                file.close();
            }

            processDownload(target, customTitle, customArtist, customCover);
        });
        return;
    }

    purgeCache();
    setStatus("Extracting stream metadata...", true);

    const QString outputPath = cacheDirectory() + "/active_stream.%(ext)s";

    QString query = target;
    if (!target.startsWith("http://") && !target.startsWith("https://")) {
        query = QString("ytsearch1:%1").arg(target);
    }

    QStringList args;
    args << query
         << "-f" << "ba/b"
         << "--no-playlist"
         << "--no-warnings"
         << "-o" << outputPath;

    m_process.disconnect();

    connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, customTitle, customArtist, customCover](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::CrashExit || exitCode != 0) {
            const QString err = QString::fromUtf8(m_process.readAllStandardError());
            setStatus(err.isEmpty() ? "Stream download failed" : err.left(100), false);
            emit streamError("Download process failed");
            return;
        }

        QDir dir(cacheDirectory());
        const QStringList files = dir.entryList(QDir::Files);

        if (!files.isEmpty()) {
            const QString fullPath = dir.absoluteFilePath(files.first());
            setStatus("Stream ready", false);
            const QString finalTitle = customTitle.isEmpty() ? "Online Stream" : customTitle;
            const QString finalArtist = customArtist.isEmpty() ? "YouTube" : customArtist;
            emit streamReady(fullPath, finalTitle, finalArtist, customCover);
        } else {
            setStatus("Stream file not found", false);
            emit streamError("Output file missing");
        }
    });

    m_process.start(ytDlp, args);
}

} // namespace Network