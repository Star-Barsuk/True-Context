#pragma once

#include <cstddef>

namespace truecontext::io {

/// Writes UTF-8 bytes to a POSIX file descriptor (default: stdout).
class FdWriter {
public:
    explicit FdWriter(int fd = 1) noexcept;

    void write(const char* data, std::size_t len);
    void write_cstr(const char* s);
    void write_line(const char* s);

    [[nodiscard]] int fd() const noexcept {
        return fd_;
    }

private:
    int fd_;
};

using StdoutWriter = FdWriter;
using StderrWriter = FdWriter;

} // namespace truecontext::io
