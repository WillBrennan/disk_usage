#include "disker/platform/unix.h"

#include <stdlib.h>
#include <sys/mount.h>

#include <iostream>

namespace disker {
UnixPlatform::UnixPlatform() = default;

UnixPlatform::~UnixPlatform() = default;

std::vector<PlatformInterface::Path> UnixPlatform::drives() const { return {}; }

std::vector<PlatformInterface::Path> UnixPlatform::favourites() const {
    const auto home = Path(getenv("HOME"));

    std::cout << "home: " << home << '\n';

    std::vector<Path> paths = {
        home, home / "Downloads", home / "Documents", home / "Pictures", home / "Videos", home / "Music",
    };

    const auto fn_not_exist = [](const Path& path) { return !std::filesystem::exists(path); };
    paths.erase(std::remove_if(paths.begin(), paths.end(), fn_not_exist), paths.end());

    return paths;
}
}  // namespace disker