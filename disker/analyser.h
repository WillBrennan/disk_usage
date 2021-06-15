#pragma once

#include <filesystem>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "disker/helpers/flat_tree.h"
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
        using Size = std::uintmax_t;
        Path path;
        Size size = 0;

        std::string name() const { return path.filename().string(); }
    };

    struct State {
        FlatTree<File> tree;
        double percentage = 0.0;  // note(will.brennan) - 0 to 1
        bool completed = false;
    };

    using Fn = std::function<void(const State&)>;

    explicit DiskAnalyser(Path path);

    ~DiskAnalyser();

    template <class Fn>
    auto state(Fn&& fn) const {
        return sync_state_.withRLock(fn);
    }

  private:
    Synchronized<State> sync_state_ = State{};
    std::thread thread_;
};

std::ostream& operator<<(std::ostream& stream, const DiskAnalyser::File& file);

namespace detail {
DiskAnalyser::File::Size total_size(const std::vector<DiskAnalyser::File>::const_iterator& begin,
                                    const std::vector<DiskAnalyser::File>::const_iterator& end);

FlatTree<DiskAnalyser::File> analyse(const DiskAnalyser::Path& path);
}  // namespace detail
}  // namespace disker