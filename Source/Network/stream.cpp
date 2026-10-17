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
    : QObject(parent) {}

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

    connect(reply, &QNetworkReply::finished, this, [this, reply, spotifyUrl]() {
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
        setStatus("Downloading official yt-dlp binary...", true);
        const QUrl downloadUrl("https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp.exe");
        QNetworkRequest req(downloadUrl);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply* reply = m_netManager.get(req);
        connect(reply, &QNetworkReply::finished, this, [this, reply, target, customTitle, customArtist, customCover]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError) {
                setStatus("Failed to download yt-dlp", false);
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
    setStatus("Fetching video info...", true);

    QString query = target;
    if (!target.startsWith("http://") && !target.startsWith("https://")) {
        query = QString("ytsearch1:%1").arg(target);
    }

    auto* metaProc = new QProcess(this);
    QStringList metaArgs;
    metaArgs << query
             << "--dump-json"
             << "--no-warnings"
             << "--no-playlist"
             << "--force-ipv4"
             << "--extractor-args" << "youtube:player_client=android,web_embedded";

    connect(metaProc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, metaProc, target, customTitle, customArtist, customCover, ytDlp, query](int exitCode, QProcess::ExitStatus) {
        metaProc->deleteLater();

        QString metaTitle = customTitle;
        QString metaArtist = customArtist;
        QString metaCover = customCover;
        QString metaId = "";

        if (exitCode == 0) {
            const QByteArray data = metaProc->readAllStandardOutput();
            const auto doc = QJsonDocument::fromJson(data);
            if (doc.isObject()) {
                const auto obj = doc.object();
                if (metaTitle.isEmpty()) {
                    metaTitle = obj.value("title").toString();
                }
                if (metaArtist.isEmpty()) {
                    metaArtist = obj.value("uploader").toString();
                    if (metaArtist.isEmpty()) metaArtist = obj.value("channel").toString();
                }
                if (metaCover.isEmpty()) {
                    metaCover = obj.value("thumbnail").toString();
                }
                metaId = obj.value("id").toString();
            }
        }

        if (metaTitle.isEmpty()) metaTitle = "Online Stream";
        if (metaArtist.isEmpty()) metaArtist = "YouTube";
        if (metaId.isEmpty()) metaId = QString::number(qHash(target));

        const QString outputPath = cacheDirectory() + "/" + metaId + ".mp3";

        if (QFile::exists(outputPath)) {
            setStatus("Ready", false);
            downloadCoverAndFinish(metaCover, outputPath, metaTitle, metaArtist, target, metaId);
            return;
        }

        setStatus("Downloading audio stream...", true);

        QStringList dlArgs;
        dlArgs << query
               << "-x"
               << "--audio-format" << "mp3"
               << "--no-playlist"
               << "--no-warnings"
               << "--force-ipv4"
               << "--extractor-args" << "youtube:player_client=android,web_embedded"
               << "-o" << outputPath;

        if (metaCover.isEmpty() && !metaId.isEmpty()) {
            metaCover = QString("https://i.ytimg.com/vi/%1/hqdefault.jpg").arg(metaId);
        } else if (metaCover.contains("vi_webp")) {
            metaCover.replace("vi_webp", "vi");
            metaCover.replace(".webp", ".jpg");
        }

        m_process.disconnect();

        connect(&m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [this, outputPath, target, metaTitle, metaArtist, metaCover, metaId](int dlExitCode, QProcess::ExitStatus dlExitStatus) {
            if (dlExitStatus == QProcess::CrashExit || dlExitCode != 0) {
                const QString err = QString::fromUtf8(m_process.readAllStandardError());
                setStatus(err.isEmpty() ? "Download failed" : err.left(100), false);
                emit streamError("yt-dlp download failed");
                return;
            }

            if (QFile::exists(outputPath)) {
                setStatus("Ready", false);
                downloadCoverAndFinish(metaCover, outputPath, metaTitle, metaArtist, target, metaId);
            } else {
                setStatus("Output file missing", false);
                emit streamError("Missing audio file");
            }
        });

        m_process.start(ytDlp, dlArgs);
    });

    metaProc->start(ytDlp, metaArgs);
}

void StreamManager::downloadCoverAndFinish(const QString& coverUrl, const QString& audioPath, const QString& title, const QString& artist, const QString& originalUrl, const QString& trackId) {
    QString targetCoverUrl = coverUrl;

    if (!trackId.isEmpty()) {
        targetCoverUrl = QString("https://i.ytimg.com/vi/%1/hqdefault.jpg").arg(trackId);
    } else if (targetCoverUrl.contains("vi_webp")) {
        targetCoverUrl.replace("vi_webp", "vi");
        targetCoverUrl.replace(".webp", ".jpg");
    }

    if (targetCoverUrl.isEmpty()) {
        emit streamReady(audioPath, title, artist, "", originalUrl, "");
        return;
    }

    const QString coverPath = cacheDirectory() + "/" + trackId + ".jpg";
    if (QFile::exists(coverPath)) {
        emit streamReady(audioPath, title, artist, coverPath, originalUrl, targetCoverUrl);
        return;
    }

    QNetworkRequest req{QUrl(targetCoverUrl)};
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply* reply = m_netManager.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, coverPath, audioPath, title, artist, originalUrl, targetCoverUrl]() {
        reply->deleteLater();
        if (reply->error() == QNetworkReply::NoError) {
            QFile file(coverPath);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(reply->readAll());
                file.close();
            }
        }
        emit streamReady(audioPath, title, artist, coverPath, originalUrl, targetCoverUrl);
    });
}

} // namespace Network