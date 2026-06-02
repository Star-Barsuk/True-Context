#pragma once

#include <truecontext/walk/path_list.hpp>

namespace truecontext::walk {

/// Recursively collects regular files under `root_abs` (absolute path).
class Walker {
public:
    explicit Walker(const char* root_abs) noexcept;

    [[nodiscard]] PathList collect() const;

    [[nodiscard]] const char* root() const noexcept {
        return root_abs_;
    }

private:
    const char* root_abs_;
};

} // namespace truecontext::walk
