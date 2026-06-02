#include <truecontext/io/stdout_writer.hpp>

#include <cstring>

#include <unistd.h>

namespace truecontext::io {

FdWriter::FdWriter(int fd) noexcept : fd_{fd} {}

void FdWriter::write(const char* data, std::size_t len) {
    if (len == 0U) {
        return;
    }
    (void)::write(fd_, data, len);
}

void FdWriter::write_cstr(const char* s) {
    if (s == nullptr) {
        return;
    }
    write(s, std::strlen(s));
}

void FdWriter::write_line(const char* s) {
    write_cstr(s);
    write("\n", 1U);
}

} // namespace truecontext::io
