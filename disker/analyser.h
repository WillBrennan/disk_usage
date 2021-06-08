#pragma once

#include <filesystem>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "disker/helpers/synchronized.h"

namespace disker {
class PlatformInterface {
  public:
    using Ptr = std::unique_ptr<PlatformInterface>;
    using Path = std::filesystem::path;

    virtual ~PlatformInterface() = default;

    virtual std::string platform() const = 0;
    virtual std::vector<Path> drives() const = 0;
    virtual std::vector<Path> favourites() const = 0;
};

PlatformInterface& platform();

class DiskAnalyser {
  public:
    using Path = PlatformInterface::Path;

    struct File {
        Path path;
        std::uintmax_t size = 0;

        std::string name() const { return path.filename().string(); }
    };

    struct Folder : File {
        std::vector<File> files;
        std::vector<Folder> folders;
    };

    struct State {
        std::optional<Folder> folder;
        double percentage;  // note(will.brennan) - 0 to 1
        bool completed;
    };

    using Fn = std::function<void(const State&)>;

    explicit DiskAnalyser(Path path);

    ~DiskAnalyser();

    template <class Fn>
    auto state(Fn&& fn) const {
        return sync_state_.withRLock(fn);
    }

  private:
    Synchronized<State> sync_state_;
    std::thread thread_;
};

namespace detail {
void recurse_structure(DiskAnalyser::Folder& folder);
}  // namespace detail
}  // namespace disker