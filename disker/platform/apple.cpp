#include "disker/platform/apple.h"

#include <stdlib.h>
#include <sys/mount.h>

#include <iostream>

namespace disker {
ApplePlatform::ApplePlatform() = default;

ApplePlatform::~ApplePlatform() = default;

std::vector<PlatformInterface::Path> ApplePlatform::drives() const { return {}; }

std::vector<PlatformInterface::Path> ApplePlatform::favourites() const {
    const auto home = Path(getenv("HOME"));

    std::vector<Path> paths = {
        home, home / "downloads", home / "documents", home / "pictures", home / "novies", home / "music",
    };

    const auto fn_not_exist = [](const Path& path) { return !std::filesystem::exists(path); };
    paths.erase(std::remove_if(paths.begin(), paths.end(), fn_not_exist), paths.end());

    return paths;
}
}  // namespace disker