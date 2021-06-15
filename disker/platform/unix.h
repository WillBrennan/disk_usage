#pragma once

#include "disker/analyser.h"

namespace disker {
class UnixPlatform final : public PlatformInterface {
  public:
    UnixPlatform();
    ~UnixPlatform();

    std::string platform() const { return "unix"; }
    std::vector<Path> drives() const;
    std::vector<Path> favourites() const;
};
}  // namespace disker