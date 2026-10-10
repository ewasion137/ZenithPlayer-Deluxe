#include "find.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <string_view>

namespace Player {

namespace {

bool isAudioFile(const std::filesystem::path& path) {
    static constexpr std::string_view extensions[] = {
        ".mp3", ".wav", ".flac", ".ogg",
        ".m4a", ".aac", ".opus", ".wma",
        ".aiff", ".aif", ".alac"
    };

    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    for (const auto& validExt : extensions) {
        if (ext == validExt) {
            return true;
        }
    }
    return false;
}

} // namespace

std::vector<FolderGroup> scanDirectory(const std::filesystem::path& rootPath) {
    std::error_code ec;
    if (!std::filesystem::exists(rootPath, ec) || !std::filesystem::is_directory(rootPath, ec)) {
        return {};
    }

    std::map<std::filesystem::path, std::vector<TrackInfo>> groupedTracks;

    const auto options = std::filesystem::directory_options::skip_permission_denied;
    auto iterator = std::filesystem::recursive_directory_iterator(rootPath, options, ec);

    for (const auto& entry : iterator) {
        if (ec) {
            ec.clear();
            continue;
        }

        if (entry.is_regular_file(ec) && isAudioFile(entry.path())) {
            const auto filePath = entry.path();
            const auto parentPath = filePath.parent_path();

            TrackInfo track{
                .name = filePath.stem().string(),
                .fullPath = filePath.string(),
                .extension = filePath.extension().string()
            };

            groupedTracks[parentPath].push_back(std::move(track));
        }
    }

    std::vector<FolderGroup> result;
    result.reserve(groupedTracks.size());

    for (auto& [folderPath, tracks] : groupedTracks) {
        std::sort(tracks.begin(), tracks.end(), [](const TrackInfo& a, const TrackInfo& b) {
            return a.name < b.name;
        });

        result.push_back(FolderGroup{
            .folderName = folderPath.filename().string(),
            .fullPath = folderPath.string(),
            .tracks = std::move(tracks)
        });
    }

    return result;
}

} // namespace Player