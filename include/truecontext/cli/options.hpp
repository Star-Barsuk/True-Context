#pragma once

namespace truecontext::cli {

enum class Action {
    Help,
    Version,
    Collect,
};

struct Options {
    Action action{Action::Help};

    /// Root directory for collection; valid for `Action::Collect` only.
    /// Points into `argv` — must outlive the process.
    const char* root_path{"."};

    /// Set when parsing fails; message suitable for stderr.
    const char* error{nullptr};
};

[[nodiscard]] inline bool parse_ok(const Options& o) noexcept {
    return o.error == nullptr;
}

} // namespace truecontext::cli
