#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace Player {

struct TrackInfo {
    std::string name;
    std::string fullPath;
    std::string extension;
};

struct FolderGroup {
    std::string folderName;
    std::string fullPath;
    std::vector<TrackInfo> tracks;
};

std::vector<FolderGroup> scanDirectory(const std::filesystem::path& rootPath);

} // namespace Player