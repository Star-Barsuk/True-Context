#include <truecontext/walk/path_list.hpp>

#include <cstdlib>
#include <cstring>

namespace truecontext::walk {

PathList::~PathList() {
    clear();
    std::free(paths_);
    paths_ = nullptr;
    capacity_ = 0;
}

PathList::PathList(PathList&& other) noexcept
    : paths_{other.paths_}, size_{other.size_}, capacity_{other.capacity_} {
    other.paths_ = nullptr;
    other.size_ = 0U;
    other.capacity_ = 0U;
}

PathList& PathList::operator=(PathList&& other) noexcept {
    if (this != &other) {
        clear();
        std::free(paths_);
        paths_ = other.paths_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.paths_ = nullptr;
        other.size_ = 0U;
        other.capacity_ = 0U;
    }
    return *this;
}

void PathList::grow() {
    const std::size_t next = (capacity_ == 0U) ? 64U : capacity_ * 2U;
    char** grown = static_cast<char**>(std::realloc(paths_, next * sizeof(char*)));
    if (grown == nullptr) {
        return;
    }
    paths_ = grown;
    capacity_ = next;
}

void PathList::push_copy(const char* path) {
    if (path == nullptr) {
        return;
    }
    if (size_ >= capacity_) {
        grow();
        if (size_ >= capacity_) {
            return;
        }
    }
    char* copy = static_cast<char*>(std::malloc(std::strlen(path) + 1U));
    if (copy == nullptr) {
        return;
    }
    std::strcpy(copy, path);
    paths_[size_] = copy;
    ++size_;
}

const char* PathList::get(std::size_t index) const {
    if (index >= size_) {
        return nullptr;
    }
    return paths_[index];
}

void PathList::clear() {
    for (std::size_t i = 0U; i < size_; ++i) {
        std::free(paths_[i]);
        paths_[i] = nullptr;
    }
    size_ = 0U;
}

} // namespace truecontext::walk
