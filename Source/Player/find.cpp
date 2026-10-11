#include "find.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <algorithm>
#include <map>

namespace Player {

std::vector<FolderGroup> scanDirectory(const QString& rootPath) {
    if (rootPath.isEmpty()) {
        return {};
    }

    static const QStringList filters = {
        "*.mp3", "*.wav", "*.flac", "*.ogg",
        "*.m4a", "*.aac", "*.opus", "*.wma",
        "*.aiff", "*.aif", "*.alac"
    };

    QDirIterator it(rootPath, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    std::map<QString, std::vector<TrackInfo>> groupedTracks;

    while (it.hasNext()) {
        it.next();
        const QFileInfo fileInfo = it.fileInfo();
        const QString parentDir = fileInfo.dir().absolutePath();

        groupedTracks[parentDir].push_back(TrackInfo{
            .name = fileInfo.completeBaseName(),
            .fullPath = fileInfo.absoluteFilePath(),
            .extension = fileInfo.suffix().toLower()
        });
    }

    std::vector<FolderGroup> result;
    result.reserve(groupedTracks.size());

    for (auto& [folderPath, tracks] : groupedTracks) {
        std::sort(tracks.begin(), tracks.end(), [](const TrackInfo& a, const TrackInfo& b) {
            return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
        });

        QDir folderDir(folderPath);
        result.push_back(FolderGroup{
            .folderName = folderDir.dirName(),
            .fullPath = folderPath,
            .tracks = std::move(tracks)
        });
    }

    return result;
}

} // namespace Player