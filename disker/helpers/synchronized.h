#pragma once

#include <algorithm>
#include <mutex>
#include <thread>

namespace disker {
// note(will.brennan) - simple version of folly::synchronized
template <class T>
class Synchronized {
  public:
    Synchronized(T value) : value_(std::move(value)) {}

    Synchronized(const Synchronized&) = delete;
    Synchronized& operator=(const Synchronized&) = delete;
    Synchronized(Synchronized&&) = delete;
    Synchronized& operator=(Synchronized&&) = delete;

    template <class Fn>
    auto withWLock(Fn&& fn) {
        std::lock_guard lock(mutex_);
        return fn(std::ref(value_));
    }

    template <class Fn>
    auto withRLock(Fn&& fn) const {
        std::lock_guard lock(mutex_);
        return fn(std::cref(value_));
    }

    T copy() const {
        std::lock_guard lock(mutex_);
        return value_;
    }

    void swap(T& other) {
        std::lock_guard lock(mutex_);
        std::swap(value_, other);
    }

  private:
    mutable std::mutex mutex_;
    T value_;
};
}  // namespace disker