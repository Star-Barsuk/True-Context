#pragma once

#include <cstddef>

namespace truecontext::walk {

/// Growable list of owned path strings (no std::vector).
class PathList {
public:
    PathList() noexcept = default;
    ~PathList();

    PathList(const PathList&) = delete;
    PathList& operator=(const PathList&) = delete;

    PathList(PathList&& other) noexcept;
    PathList& operator=(PathList&& other) noexcept;

    void push_copy(const char* path);
    [[nodiscard]] std::size_t size() const noexcept {
        return size_;
    }
    [[nodiscard]] bool empty() const noexcept {
        return size_ == 0U;
    }
    [[nodiscard]] const char* get(std::size_t index) const;
    [[nodiscard]] char** data() noexcept {
        return paths_;
    }
    void clear();

private:
    void grow();

    char** paths_{nullptr};
    std::size_t size_{0};
    std::size_t capacity_{0};
};

} // namespace truecontext::walk
