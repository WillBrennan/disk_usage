#pragma once

#include <memory>
#include <string_view>

namespace disker {
namespace logger {

struct LoggerInterface {
    using Ptr = std::unique_ptr<LoggerInterface>;

    virtual ~LoggerInterface() {}

    virtual void debug(const std::string_view& msg) = 0;
    virtual void info(const std::string_view& msg) = 0;
    virtual void warn(const std::string_view& msg) = 0;
    virtual void error(const std::string_view& msg) = 0;
};

void setLogger(LoggerInterface::Ptr logger);

void error(const std::string_view& msg);

void warn(const std::string_view& msg);

void info(const std::string_view& msg);

void debug(const std::string_view& msg);

void check(bool passes, const std::string_view& msg);
}  // namespace logger
}  // namespace disker