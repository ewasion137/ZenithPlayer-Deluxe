#include "library.hpp"
#include "find.hpp"
#include "metadata.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>
#include <QVariantMap>

namespace Player {

Library::Library(QObject* parent)
    : QObject(parent) {}

QVariantList Library::folders() const {
    if (m_searchQuery.trimmed().isEmpty()) {
        return m_rawFolders;
    }

    QVariantList filtered;
    for (const auto& fVal : m_rawFolders) {
        const auto fMap = fVal.toMap();
        const auto tracks = fMap["tracks"].toList();
        QVariantList matchedTracks;

        for (const auto& tVal : tracks) {
            const auto tMap = tVal.toMap();
            if (tMap["name"].toString().contains(m_searchQuery, Qt::CaseInsensitive)) {
                matchedTracks.append(tMap);
            }
        }

        if (!matchedTracks.isEmpty()) {
            QVariantMap copy = fMap;
            copy["tracks"] = matchedTracks;
            filtered.append(copy);
        }
    }
    return filtered;
}

QString Library::currentTrackPath() const {
    return m_currentTrackPath;
}

QString Library::currentTitle() const {
    return m_currentTitle;
}

QString Library::currentArtist() const {
    return m_currentArtist;
}

QString Library::currentAlbum() const {
    return m_currentAlbum;
}

QString Library::currentCoverUrl() const {
    return m_currentCoverUrl;
}

QString Library::searchQuery() const {
    return m_searchQuery;
}

QImage Library::getCoverImage(const QString& path) const {
    Q_UNUSED(path);
    return m_cachedCover;
}

void Library::setSearchQuery(const QString& query) {
    if (m_searchQuery != query) {
        m_searchQuery = query;
        emit searchQueryChanged();
        emit foldersChanged();
    }
}

void Library::setCurrentTrackPath(const QString& path) {
    if (m_currentTrackPath == path) {
        return;
    }

    m_currentTrackPath = path;

    if (!path.contains("WebStream")) {
        updateMetadata(path);
    }

    emit currentTrackPathChanged();
}

void Library::setWebMetadata(const QString& title, const QString& artist, const QString& coverUrl) {
    m_currentTitle = title;
    m_currentArtist = artist;
    m_currentAlbum = "Online Stream";
    m_currentCoverUrl = coverUrl;

    emit metadataChanged();
}

void Library::updateMetadata(const QString& path) {
    const TrackMetadata meta = extractMetadata(path);
    m_currentTitle = meta.title;
    m_currentArtist = meta.artist;
    m_currentAlbum = meta.album;
    m_cachedCover = meta.cover;

    static qint64 counter = 0;
    m_currentCoverUrl = meta.cover.isNull() ? "" : QString("image://covers/%1").arg(++counter);

    emit metadataChanged();
}

void Library::selectFolder() {
    const QString dir = QFileDialog::getExistingDirectory(
        nullptr,
        "Select Music Directory",
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dir.isEmpty()) {
        scanPath(dir);
    }
}

void Library::scanPath(const QString& rawPath) {
    QString path = rawPath;
    if (path.startsWith("file:///")) {
        path = QUrl(rawPath).toLocalFile();
    }

    const auto groups = scanDirectory(path);

    QVariantList folderList;
    folderList.reserve(static_cast<qsizetype>(groups.size()));

    for (const auto& group : groups) {
        QVariantMap folderMap;
        folderMap["folderName"] = group.folderName;
        folderMap["fullPath"] = group.fullPath;

        QVariantList trackList;
        trackList.reserve(static_cast<qsizetype>(group.tracks.size()));

        for (const auto& track : group.tracks) {
            QVariantMap trackMap;
            trackMap["name"] = track.name;
            trackMap["fullPath"] = track.fullPath;
            trackMap["extension"] = track.extension;
            trackList.append(trackMap);
        }

        folderMap["tracks"] = trackList;
        folderList.append(folderMap);
    }

    m_rawFolders = folderList;
    emit foldersChanged();
}

QString Library::getNextTrack() const {
    const auto list = folders();
    if (list.isEmpty() || m_currentTrackPath.isEmpty()) {
        return {};
    }

    QList<QString> allPaths;
    for (const auto& folderVal : list) {
        const auto folderMap = folderVal.toMap();
        const auto tracks = folderMap["tracks"].toList();
        for (const auto& trackVal : tracks) {
            allPaths.append(trackVal.toMap()["fullPath"].toString());
        }
    }

    const int idx = allPaths.indexOf(m_currentTrackPath);
    if (idx != -1 && idx + 1 < allPaths.size()) {
        return allPaths[idx + 1];
    }
    return {};
}

QString Library::getPreviousTrack() const {
    const auto list = folders();
    if (list.isEmpty() || m_currentTrackPath.isEmpty()) {
        return {};
    }

    QList<QString> allPaths;
    for (const auto& folderVal : list) {
        const auto folderMap = folderVal.toMap();
        const auto tracks = folderMap["tracks"].toList();
        for (const auto& trackVal : tracks) {
            allPaths.append(trackVal.toMap()["fullPath"].toString());
        }
    }

    const int idx = allPaths.indexOf(m_currentTrackPath);
    if (idx > 0) {
        return allPaths[idx - 1];
    }
    return {};
}

} // namespace Player