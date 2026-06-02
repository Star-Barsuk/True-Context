#pragma once

namespace truecontext::render {

/// Node in an in-memory directory tree (sibling list, no std::vector).
struct TreeNode {
    char* name{nullptr};
    bool is_dir{false};
    TreeNode* first_child{nullptr};
    TreeNode* next_sibling{nullptr};
};

} // namespace truecontext::render
