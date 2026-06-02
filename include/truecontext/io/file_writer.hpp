#pragma once

#include <cstddef>

namespace truecontext::io {

/// Append-oriented file writer (stdio, not iostream).
class FileWriter {
public:
    explicit FileWriter(const char* path);
    ~FileWriter();

    FileWriter(const FileWriter&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;

    [[nodiscard]] bool ok() const noexcept {
        return file_ != nullptr;
    }

    void write(const char* data, std::size_t len);
    void write_cstr(const char* s);
    void write_line(const char* s);

private:
    void* file_{nullptr};
};

} // namespace truecontext::io
