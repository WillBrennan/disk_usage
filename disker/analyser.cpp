#include "disker/analyser.h"

#ifdef _WIN32
#include "disker/platform/windows.h"
#elif __APPLE__
#include "disker/platform/apple.h"
#elif __linux__
#include "disker/platform/unix.h"
#endif

#include <numeric>

#include "disker/logging.h"

namespace disker {
PlatformInterface& platform() {
    static PlatformInterface::Ptr platform;

    if (platform == nullptr) {
#ifdef _WIN32
        platform = std::make_unique<WindowsPlatform>();
#elif __APPLE__
        platform = std::make_unique<ApplePlatform>();
#elif __linux__
        platform = std::make_unique<UnixPlatform>();
#endif
    }

    return *platform;
}

DiskAnalyser::DiskAnalyser(Path path) {
    thread_ = std::thread([this, path] {
        auto folder = Folder{path};
        detail::recurse_structure(folder);
        sync_state_.withWLock([&folder](State& state) { state.folder = std::move(folder); });
    });
}

DiskAnalyser::~DiskAnalyser() { thread_.join(); }

namespace detail {
void recurse_structure(DiskAnalyser::Folder& folder) {
    for (auto& entry : std::filesystem::directory_iterator(folder.path)) {
        const auto& path = entry.path();
        if (std::filesystem::is_directory(path)) {
            auto& subfolder = folder.folders.emplace_back(DiskAnalyser::Folder{path});
            recurse_structure(subfolder);
            folder.size += subfolder.size;
        } else if (std::filesystem::is_regular_file(path)) {
            auto file = DiskAnalyser::File{};
            file.path = path;
            file.size = std::filesystem::file_size(path);
            folder.files.emplace_back(file);

            folder.size += file.size;
        } else {
            logger::warn("skipping " + path.filename().string() + " not directory or file");
        }
    }

    const auto fn_size = [](const DiskAnalyser::File& lhs, const DiskAnalyser::File& rhs) {
        return lhs.size < rhs.size;
    };

    std::sort(folder.folders.rbegin(), folder.folders.rend(), fn_size);
    std::sort(folder.files.rbegin(), folder.files.rend(), fn_size);
}
}  // namespace detail
}  // namespace disker