#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <stdlib.h>

#include "disker/platform/unix.h"

TEST(UnixPlatform, constructor) {
    using disker::UnixPlatform;
    auto platform = UnixPlatform();
}

TEST(UnixPlatform, platform) {
    using disker::UnixPlatform;
    const auto platform = UnixPlatform();
    EXPECT_EQ(platform.platform(), "apple");
}

TEST(UnixPlatform, drives) {
    using disker::UnixPlatform;
    const auto platform = UnixPlatform();

    const auto drives = platform.drives();
}

TEST(UnixPlatform, favourites) {
    using disker::UnixPlatform;
    const auto platform = UnixPlatform();

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