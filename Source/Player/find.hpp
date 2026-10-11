#pragma once

#include <QString>
#include <vector>

namespace Player {

struct TrackInfo {
    QString name;
    QString fullPath;
    QString extension;
};

struct FolderGroup {
    QString folderName;
    QString fullPath;
    std::vector<TrackInfo> tracks;
};

std::vector<FolderGroup> scanDirectory(const QString& rootPath);

} // namespace Player