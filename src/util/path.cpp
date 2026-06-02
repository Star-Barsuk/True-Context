#include <truecontext/util/path.hpp>

#include <cstdio>
#include <cstring>
#include <sys/stat.h>

namespace truecontext::util {

bool path_join(char* out, std::size_t out_size, const char* base, const char* relative) {
    if (out_size == 0U || base == nullptr || relative == nullptr) {
        return false;
    }

    const std::size_t base_len = std::strlen(base);
    const bool needs_slash = base_len > 0U && base[base_len - 1U] != '/';
    const int written = std::snprintf(
        out,
        out_size,
        needs_slash ? "%s/%s" : "%s%s",
        base,
        relative
    );
    return written > 0 && static_cast<std::size_t>(written) < out_size;
}

bool ensure_parent_dirs(const char* file_path) {
    if (file_path == nullptr) {
        return false;
    }

    char dir[4096];
    std::strncpy(dir, file_path, sizeof(dir) - 1U);
    dir[sizeof(dir) - 1U] = '\0';

    char* slash = std::strrchr(dir, '/');
    if (slash == nullptr) {
        return true;
    }
    *slash = '\0';
    if (dir[0] == '\0') {
        return true;
    }

    char partial[4096];
    partial[0] = '\0';

    char* segment = dir;
    if (dir[0] == '/') {
        partial[0] = '/';
        partial[1] = '\0';
        segment = dir + 1;
    }

    while (segment != nullptr && *segment != '\0') {
        char* next = std::strchr(segment, '/');
        if (next != nullptr) {
            *next = '\0';
        }

        const std::size_t plen = std::strlen(partial);
        if (plen + std::strlen(segment) + 2U >= sizeof(partial)) {
            return false;
        }
        if (plen > 0U && partial[plen - 1U] != '/') {
            std::strcat(partial, "/");
        }
        std::strcat(partial, segment);

        struct stat st {};
        if (stat(partial, &st) != 0) {
            if (mkdir(partial, 0755) != 0 && stat(partial, &st) != 0) {
                return false;
            }
        }

        if (next == nullptr) {
            break;
        }
        *next = '/';
        segment = next + 1;
    }

    return true;
}

bool is_binary_content(const unsigned char* data, std::size_t len) noexcept {
    if (data == nullptr || len == 0U) {
        return false;
    }
    std::size_t check = len < 8192U ? len : 8192U;
    for (std::size_t i = 0U; i < check; ++i) {
        if (data[i] == 0U) {
            return true;
        }
    }
    return false;
}

} // namespace truecontext::util
