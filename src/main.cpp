#include <truecontext/version.hpp>

#include <cstddef>
#include <cstring>

#include <unistd.h>

namespace {

void write_bytes(const char* data, std::size_t len) {
    (void)::write(STDOUT_FILENO, data, len);
}

void write_cstr(const char* s) {
    write_bytes(s, std::strlen(s));
}

void write_line(const char* s) {
    write_cstr(s);
    write_bytes("\n", 1);
}

bool streq(const char* a, const char* b) {
    return std::strcmp(a, b) == 0;
}

int print_help() {
    write_line("Usage: true_context [PATH]");
    write_line("       true_context --version");
    write_line("       true_context --help");
    write_line("");
    write_line("Collect project files into context.txt for LLM prompts.");
    write_line("Implementation in progress — see PROJECT_GOALS.md");
    return 0;
}

int print_version() {
    write_cstr(truecontext::kName);
    write_cstr(" ");
    write_line(truecontext::kVersion);
    write_line("(stub — collector not implemented)");
    return 0;
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        if (streq(argv[1], "--help") || streq(argv[1], "-h")) {
            return print_help();
        }
        if (streq(argv[1], "--version")) {
            return print_version();
        }
    }

    if (argc == 1) {
        return print_help();
    }

    write_line("true_context: collector not implemented yet");
    return 1;
}
