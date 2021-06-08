#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "disker/logging.h"

TEST(Logger, initialLogger) {
    using disker::logger::debug;
    using disker::logger::error;
    using disker::logger::info;
    using disker::logger::warn;

    error("here");
    warn("there");
    info("where");
    debug("somewhere");
}

TEST(Logger, defaultLogger) {
    using disker::logger::debug;
    using disker::logger::error;
    using disker::logger::info;
    using disker::logger::setLogger;
    using disker::logger::warn;

    setLogger(nullptr);
    error("here");
    warn("there");
    info("where");
    debug("somewhere");
}

TEST(Logger, setLogger) {
    using disker::logger::debug;
    using disker::logger::error;
    using disker::logger::info;
    using disker::logger::LoggerInterface;
    using disker::logger::setLogger;
    using disker::logger::warn;

    struct Logger final : LoggerInterface {
        explicit Logger(std::string* msg) : msg_(msg) {}
        ~Logger() override = default;

        void debug(const std::string_view& msg) override { *msg_ = msg; }
        void info(const std::string_view& msg) override { *msg_ = msg; }
        void warn(const std::string_view& msg) override { *msg_ = msg; }
        void error(const std::string_view& msg) override { *msg_ = msg; }

        std::string* msg_ = nullptr;
    };

    std::string msg_;
    auto logger = std::make_unique<Logger>(&msg_);
    setLogger(std::move(logger));

    error("here");
    warn("there");
    info("where");
    debug("somewhere");

    setLogger(nullptr);
}