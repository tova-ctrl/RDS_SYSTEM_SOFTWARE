#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

// Global thread-safe logger — all modules use this
inline void fcc_log(const std::string& prefix, const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    ::localtime_r(&t, &tm);
    char ts[16];
    ::strftime(ts, sizeof(ts), "%H:%M:%S", &tm);

    static std::mutex g_log_mutex;
    std::lock_guard<std::mutex> lk(g_log_mutex);
    std::cout << "[" << prefix << " " << ts << "] " << msg << "\n" << std::flush;
}

// Thread-safe circular log buffer
class CircularLog {
public:
    explicit CircularLog(size_t capacity = 1000) : capacity_(capacity) {
        buffer_.reserve(capacity);
    }

    void setLastStatus(const std::string& s) {
        std::lock_guard<std::mutex> lk(mutex_);
        last_status_ = s;
    }

    std::string lastStatus() const {
        std::lock_guard<std::mutex> lk(mutex_);
        return last_status_;
    }

    void push(const std::string& entry) {
        std::lock_guard<std::mutex> lk(mutex_);
        if (buffer_.size() < capacity_) {
            buffer_.push_back(entry);
        } else {
            buffer_[head_] = entry;
            head_ = (head_ + 1) % capacity_;
        }
    }

    // Returns all entries in chronological order
    std::vector<std::string> snapshot() const {
        std::lock_guard<std::mutex> lk(mutex_);
        if (buffer_.size() < capacity_)
            return buffer_;
        std::vector<std::string> result;
        result.reserve(capacity_);
        for (size_t i = 0; i < capacity_; ++i)
            result.push_back(buffer_[(head_ + i) % capacity_]);
        return result;
    }

    // Returns all entries in chronological order and clears the buffer
    std::vector<std::string> pop_all() {
        std::lock_guard<std::mutex> lk(mutex_);
        std::vector<std::string> result;
        if (buffer_.size() < capacity_) {
            result = std::move(buffer_);
        } else {
            result.reserve(capacity_);
            for (size_t i = 0; i < capacity_; ++i)
                result.push_back(buffer_[(head_ + i) % capacity_]);
        }
        buffer_.clear();
        head_ = 0;
        return result;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lk(mutex_);
        return buffer_.size();
    }

private:
    size_t                   capacity_;
    std::vector<std::string> buffer_;
    size_t                   head_ = 0;
    std::string              last_status_;
    mutable std::mutex       mutex_;
};
