#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "disker/platform/apple.h"

TEST(ApplePlatform, constructor) {
    using disker::ApplePlatform;
    auto platform = ApplePlatform();
}

TEST(ApplePlatform, platform) {
    using disker::ApplePlatform;
    const auto platform = ApplePlatform();
    EXPECT_EQ(platform.platform(), "apple");
}

TEST(ApplePlatform, drives) {
    using disker::ApplePlatform;
    const auto platform = ApplePlatform();

    const auto drives = platform.drives();
}

TEST(ApplePlatform, favourites) {
    using disker::ApplePlatform;
    const auto platform = ApplePlatform();

    const auto base = std::filesystem::current_path();
    const auto downloads = base / "downloads";
    const auto documents = base / "documents";
    const auto music = base / "music";

    std::filesystem::create_directories(downloads);
    std::filesystem::create_directories(documents);
    std::filesystem::create_directories(music);

    setenv("HOME", base.c_str(), true);

    const auto paths = platform.favourites();

    EXPECT_THAT(paths, testing::UnorderedElementsAre(base, downloads, documents, music));
}