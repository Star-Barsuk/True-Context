#include <truecontext/render/renderer.hpp>

#include <truecontext/render/tree_renderer.hpp>
#include <truecontext/util/path.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace truecontext::render {

Renderer::Renderer(const char* root_abs, const char* output_abs) noexcept
    : root_abs_{root_abs}, output_abs_{output_abs} {
    const char* env = std::getenv("MAX_FILE_SIZE");
    if (env != nullptr && env[0] != '\0') {
        const unsigned long long v = std::strtoull(env, nullptr, 10);
        max_file_bytes_ = static_cast<std::size_t>(v);
    }
}

bool Renderer::render(const walk::PathList& files) {
    if (root_abs_ == nullptr || output_abs_ == nullptr) {
        return false;
    }

    if (!util::ensure_parent_dirs(output_abs_)) {
        return false;
    }

    out_ = std::fopen(output_abs_, "w");
    if (out_ == nullptr) {
        return false;
    }

    write_header();
    write_tree(files);
    write_contents(files);

    std::fclose(static_cast<FILE*>(out_));
    out_ = nullptr;
    return true;
}

void Renderer::write_header() {
    std::fprintf(static_cast<FILE*>(out_), "# Project context\n\n");

    std::time_t now = std::time(nullptr);
    if (now >= 0) {
        char buf[64];
        if (std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S UTC", std::gmtime(&now)) > 0) {
            std::fprintf(static_cast<FILE*>(out_), "Generated: %s\n\n", buf);
        }
    }
}

void Renderer::write_tree(const walk::PathList& files) {
    TreeRenderer::write(static_cast<FILE*>(out_), files);
}

void Renderer::write_contents(const walk::PathList& files) {
    std::fprintf(static_cast<FILE*>(out_), "## File contents\n\n");

    for (std::size_t i = 0U; i < files.size(); ++i) {
        const char* rel = files.get(i);
        if (rel != nullptr) {
            write_file_body(rel);
        }
    }
}

void Renderer::write_file_body(const char* rel_path) {
    char abs[4096];
    if (!util::path_join(abs, sizeof(abs), root_abs_, rel_path)) {
        return;
    }

    auto* out = static_cast<FILE*>(out_);
    std::fprintf(out, "### `%s`\n\n", rel_path);

    FILE* in = std::fopen(abs, "rb");
    if (in == nullptr) {
        std::fprintf(out, "*[unable to read file]*\n\n");
        return;
    }

    unsigned char buffer[8192];
    const std::size_t peek = std::fread(buffer, 1U, sizeof(buffer), in);
    if (peek > 0U && util::is_binary_content(buffer, peek)) {
        std::fclose(in);
        std::fprintf(out, "*[BINARY FILE - SKIPPED]*\n\n");
        return;
    }

    if (max_file_bytes_ > 0U) {
        std::fseek(in, 0, SEEK_END);
        const long sz = std::ftell(in);
        if (sz >= 0 && static_cast<std::size_t>(sz) > max_file_bytes_) {
            std::fclose(in);
            std::fprintf(
                out,
                "*[FILE TOO LARGE: %ld bytes, limit %zu]*\n\n",
                sz,
                max_file_bytes_
            );
            return;
        }
    }

    std::rewind(in);
    std::fprintf(out, "```\n");

    std::size_t n = 0U;
    while ((n = std::fread(buffer, 1U, sizeof(buffer), in)) > 0U) {
        std::fwrite(buffer, 1U, n, out);
    }

    std::fclose(in);
    std::fprintf(out, "\n```\n\n");
}

} // namespace truecontext::render
