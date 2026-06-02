#pragma once

#include <truecontext/walk/path_list.hpp>

namespace truecontext::render {

/// Writes `docs/context.md` (markdown): tree + file bodies.
class Renderer {
public:
    Renderer(const char* root_abs, const char* output_abs) noexcept;

    [[nodiscard]] bool render(const walk::PathList& files);

private:
    void write_header();
    void write_tree(const walk::PathList& files);
    void write_contents(const walk::PathList& files);
    void write_file_body(const char* rel_path);

    const char* root_abs_{nullptr};
    const char* output_abs_{nullptr};
    void* out_{nullptr};
    std::size_t max_file_bytes_{500000U};
};

} // namespace truecontext::render
