#include "disker/analyser.h"

#ifdef _WIN32
#include "disker/platform/windows.h"
#elif __APPLE__
#include "disker/platform/apple.h"
#elif __linux__
#include "disker/platform/unix.h"
#endif

#include <numeric>
#include <stack>

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
        auto tree = detail::analyse(path);
        sync_state_.withWLock([tree = std::move(tree)](State& state) {
            state.tree = std::move(tree);
            state.completed = true;
        });
    });
}

DiskAnalyser::~DiskAnalyser() { thread_.join(); }

std::ostream& operator<<(std::ostream& stream, const DiskAnalyser::File& file) {
    stream << "{";
    stream << " path: " << file.path;
    stream << " size: " << file.size;
    stream << "}";
    return stream;
}

namespace detail {
DiskAnalyser::File::Size total_size(const std::vector<DiskAnalyser::File>::const_iterator& begin,
                                    const std::vector<DiskAnalyser::File>::const_iterator& end) {
    using File = DiskAnalyser::File;
    return std::accumulate(begin, end, 0u, [](File::Size acc, const File& file) { return file.size + acc; });
}

FlatTree<DiskAnalyser::File> analyse(const DiskAnalyser::Path& path) {
    using File = DiskAnalyser::File;
    namespace fs = std::filesystem;
    FlatTree<File> tree;

    std::vector<int> idx_to_process;
    idx_to_process.reserve(10);

    auto idx = tree.emplace(FlatTree<File>::NoParent, File{path, 0});
    idx_to_process.emplace_back(idx);

    std::vector<File> folders;
    std::vector<File> files;

    while (!idx_to_process.empty()) {
        auto idx = idx_to_process.back();
        idx_to_process.pop_back();

        const auto& folder = tree.at(idx);

        logger::debug("collecting children file entries");
        files.clear();
        folders.clear();

        std::error_code ec;

        for (const auto& entry : fs::directory_iterator(folder.path, ec)) {
            const auto& subpath = entry.path();

            if (fs::is_symlink(subpath)) {
                continue;
            } else if (fs::is_directory(subpath)) {
                folders.emplace_back(File{subpath, 0});
            } else if (fs::is_regular_file(subpath)) {
                const auto size = std::filesystem::file_size(subpath);
                files.emplace_back(File{subpath, size});
            }
        }

        if (ec) {
            continue;
        }

        const auto fn_by_name = [](const File& lhs, const File& rhs) { return lhs.name() < rhs.name(); };
        const auto fn_by_size = [](const File& lhs, const File& rhs) { return lhs.size > rhs.size; };
        // note(will.brennan):
        // this cant be by size... folders dont have the correct size until the tree has been fully populated
        std::sort(folders.begin(), folders.end(), fn_by_name);
        std::sort(files.begin(), files.end(), fn_by_size);

        logger::debug("adding file sizes to their parents to the root");
        const auto total_file_sizes = total_size(files.begin(), files.end());

        {
            auto parent_idx = idx;
            while (parent_idx != FlatTree<File>::NoParent) {
                auto& parent = tree.at(parent_idx);
                parent.size += total_file_sizes;
                parent_idx = tree.parent(parent_idx);
            }
        }

        logger::debug("adding children to tree");
        for (auto&& folder : folders) {
            const auto child_idx = tree.emplace(idx, std::move(folder));
            idx_to_process.emplace_back(child_idx);
        }

        for (auto&& file : files) {
            tree.emplace(idx, std::move(file));
        }
    }

    return tree;
}
}  // namespace detail
}  // namespace disker