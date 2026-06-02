#include <truecontext/render/tree_renderer.hpp>

#include <truecontext/render/tree_node.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace truecontext::render {

namespace {

TreeNode* make_node(const char* name, bool is_dir) {
    auto* node = static_cast<TreeNode*>(std::malloc(sizeof(TreeNode)));
    if (node == nullptr) {
        return nullptr;
    }
    node->name = static_cast<char*>(std::malloc(std::strlen(name) + 1U));
    if (node->name == nullptr) {
        std::free(node);
        return nullptr;
    }
    std::strcpy(node->name, name);
    node->is_dir = is_dir;
    node->first_child = nullptr;
    node->next_sibling = nullptr;
    return node;
}

void append_child(TreeNode* parent, TreeNode* child) {
    if (parent->first_child == nullptr) {
        parent->first_child = child;
        return;
    }
    TreeNode* tail = parent->first_child;
    while (tail->next_sibling != nullptr) {
        tail = tail->next_sibling;
    }
    tail->next_sibling = child;
}

TreeNode* find_child(TreeNode* parent, const char* name) {
    for (TreeNode* child = parent->first_child; child != nullptr; child = child->next_sibling) {
        if (std::strcmp(child->name, name) == 0) {
            return child;
        }
    }
    return nullptr;
}

TreeNode* ensure_dir_child(TreeNode* parent, const char* name) {
    TreeNode* child = find_child(parent, name);
    if (child != nullptr) {
        child->is_dir = true;
        return child;
    }
    child = make_node(name, true);
    if (child != nullptr) {
        append_child(parent, child);
    }
    return child;
}

void insert_path(TreeNode* root, const char* path) {
    if (path == nullptr || path[0] == '\0') {
        return;
    }

    TreeNode* current = root;
    const char* segment_start = path;

    for (;;) {
        const char* slash = std::strchr(segment_start, '/');
        const bool is_last = (slash == nullptr);

        char segment[512];
        const std::size_t len =
            is_last ? std::strlen(segment_start) : static_cast<std::size_t>(slash - segment_start);
        if (len == 0U || len >= sizeof(segment)) {
            return;
        }
        std::memcpy(segment, segment_start, len);
        segment[len] = '\0';

        if (is_last) {
            TreeNode* leaf = make_node(segment, false);
            if (leaf != nullptr) {
                append_child(current, leaf);
            }
            return;
        }

        TreeNode* dir = ensure_dir_child(current, segment);
        if (dir == nullptr) {
            return;
        }
        current = dir;
        segment_start = slash + 1;
    }
}

void destroy_tree(TreeNode* node) {
    if (node == nullptr) {
        return;
    }
    for (TreeNode* child = node->first_child; child != nullptr;) {
        TreeNode* next = child->next_sibling;
        destroy_tree(child);
        child = next;
    }
    std::free(node->name);
    std::free(node);
}

std::size_t count_children(const TreeNode* parent) {
    std::size_t count = 0U;
    for (const TreeNode* child = parent->first_child; child != nullptr; child = child->next_sibling) {
        ++count;
    }
    return count;
}

int compare_nodes(const void* a, const void* b) {
    const auto* lhs = *static_cast<TreeNode* const*>(a);
    const auto* rhs = *static_cast<TreeNode* const*>(b);
    if (lhs->is_dir != rhs->is_dir) {
        return lhs->is_dir ? -1 : 1;
    }
    return std::strcmp(lhs->name, rhs->name);
}

void write_prefix_line(FILE* out, const char* prefix, bool last, const TreeNode* node) {
    std::fprintf(
        out,
        "%s%s %s%s\n",
        prefix,
        last ? "└──" : "├──",
        node->name,
        node->is_dir ? "/" : ""
    );
}

void render_subtree(FILE* out, TreeNode* parent, const char* prefix) {
    const std::size_t child_count = count_children(parent);
    if (child_count == 0U) {
        return;
    }

    auto** children =
        static_cast<TreeNode**>(std::malloc(child_count * sizeof(TreeNode*)));
    if (children == nullptr) {
        return;
    }

    std::size_t index = 0U;
    for (TreeNode* child = parent->first_child; child != nullptr; child = child->next_sibling) {
        children[index++] = child;
    }

    std::qsort(children, child_count, sizeof(TreeNode*), compare_nodes);

    for (std::size_t i = 0U; i < child_count; ++i) {
        const bool last = (i + 1U == child_count);
        write_prefix_line(out, prefix, last, children[i]);

        if (children[i]->first_child != nullptr) {
            char child_prefix[4096];
            const int written = std::snprintf(
                child_prefix,
                sizeof(child_prefix),
                "%s%s",
                prefix,
                last ? "    " : "│   "
            );
            if (written > 0 && static_cast<std::size_t>(written) < sizeof(child_prefix)) {
                render_subtree(out, children[i], child_prefix);
            }
        }
    }

    std::free(children);
}

TreeNode* build_tree(const walk::PathList& files) {
    TreeNode* root = make_node("", true);
    if (root == nullptr) {
        return nullptr;
    }

    for (std::size_t i = 0U; i < files.size(); ++i) {
        insert_path(root, files.get(i));
    }

    return root;
}

} // namespace

void TreeRenderer::write(FILE* out, const walk::PathList& files) {
    if (out == nullptr) {
        return;
    }

    std::fprintf(out, "## Project structure\n\n");
    std::fprintf(out, "```text\n");

    TreeNode* root = build_tree(files);
    if (root != nullptr) {
        render_subtree(out, root, "");
        destroy_tree(root);
    }

    std::fprintf(out, "\nTOTAL FILES: %zu\n", files.size());
    std::fprintf(out, "```\n\n");
}

} // namespace truecontext::render
