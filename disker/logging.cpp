#include "disker/logging.h"

#include <iostream>
#include <stdexcept>

namespace disker {
namespace logger {

struct DefaultLogger final : LoggerInterface {
    ~DefaultLogger() = default;

    void debug(const std::string_view& msg) override { std::clog << "[debug] - " << msg << '\n'; }
    void info(const std::string_view& msg) override { std::clog << "[info] - " << msg << '\n'; }
    void warn(const std::string_view& msg) override { std::cerr << "[warn] - " << msg << '\n'; }
    void error(const std::string_view& msg) override { std::cerr << "[error] - " << msg << '\n'; }
};

static LoggerInterface::Ptr logger_ = std::make_unique<DefaultLogger>();

void setLogger(LoggerInterface::Ptr logger) {
    if (logger == nullptr) {
        logger_ = std::make_unique<DefaultLogger>();
    } else {
        logger_ = std::move(logger);
    }
}

void error(const std::string_view& msg) { logger_->error(msg); }

void warn(const std::string_view& msg) { logger_->warn(msg); }

void info(const std::string_view& msg) { logger_->info(msg); }

void debug(const std::string_view& msg) { logger_->debug(msg); }

void check(bool passes, const std::string_view& msg) {
    if (!passes) {
        logger::error(msg);
        throw std::runtime_error(msg.data());
    }
}

void check_eq(int a, int b, const std::string_view& msg) { check(a == b, msg); }

void check_eq(std::uintmax_t a, std::uintmax_t b, const std::string_view& msg) { check(a == b, msg); }
}  // namespace logger
}  // namespace disker