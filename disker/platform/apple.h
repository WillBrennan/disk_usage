#pragma once

#include "disker/analyser.h"

namespace disker {
class ApplePlatform final : public PlatformInterface {
  public:
    ApplePlatform();
    ~ApplePlatform();

    std::string platform() const { return "apple"; }
    std::vector<Path> drives() const;
    std::vector<Path> favourites() const;
};
}  // namespace disker