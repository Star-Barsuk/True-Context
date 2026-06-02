#include <truecontext/io/file_writer.hpp>

#include <cstdio>
#include <cstring>

namespace truecontext::io {

FileWriter::FileWriter(const char* path) {
    if (path != nullptr) {
        file_ = std::fopen(path, "w");
    }
}

FileWriter::~FileWriter() {
    if (file_ != nullptr) {
        std::fclose(static_cast<FILE*>(file_));
        file_ = nullptr;
    }
}

void FileWriter::write(const char* data, std::size_t len) {
    if (file_ == nullptr || data == nullptr || len == 0U) {
        return;
    }
    (void)std::fwrite(data, 1U, len, static_cast<FILE*>(file_));
}

void FileWriter::write_cstr(const char* s) {
    if (s != nullptr) {
        write(s, std::strlen(s));
    }
}

void FileWriter::write_line(const char* s) {
    write_cstr(s);
    write("\n", 1U);
}

} // namespace truecontext::io
