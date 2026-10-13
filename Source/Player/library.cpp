#include "library.hpp"
#include "find.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>
#include <QVariantMap>

namespace Player {

Library::Library(QObject* parent)
    : QObject(parent) {}

QVariantList Library::folders() const {
    return m_folders;
}

QString Library::currentTrackPath() const {
    return m_currentTrackPath;
}

QString Library::currentTrackName() const {
    return m_currentTrackName;
}

void Library::setCurrentTrackPath(const QString& path) {
    if (m_currentTrackPath == path) {
        return;
    }

    m_currentTrackPath = path;
    m_currentTrackName = QFileInfo(path).completeBaseName();

    emit currentTrackPathChanged();
    emit currentTrackNameChanged();
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

    m_folders = folderList;
    emit foldersChanged();
}

QString Library::getNextTrack() const {
    if (m_folders.isEmpty() || m_currentTrackPath.isEmpty()) {
        return {};
    }

    QList<QString> allPaths;
    for (const auto& folderVal : m_folders) {
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
    if (m_folders.isEmpty() || m_currentTrackPath.isEmpty()) {
        return {};
    }

    QList<QString> allPaths;
    for (const auto& folderVal : m_folders) {
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