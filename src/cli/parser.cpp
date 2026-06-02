#include <truecontext/cli/parser.hpp>

#include <truecontext/util/cstring.hpp>

namespace truecontext::cli {

namespace {

void set_error(Options& out, const char* message) {
    out.error = message;
}

} // namespace

Options Parser::parse(int argc, char* argv[]) {
    Options out{};

    if (argc <= 1) {
        out.action = Action::Help;
        return out;
    }

    const char* arg = argv[1];

    if (util::streq(arg, "--help") || util::streq(arg, "-h")) {
        out.action = Action::Help;
        return out;
    }

    if (util::streq(arg, "--version")) {
        out.action = Action::Version;
        return out;
    }

    if (util::starts_with(arg, '-')) {
        set_error(out, "unknown option (try --help)");
        return out;
    }

    out.action = Action::Collect;
    out.root_path = arg;

    if (argc > 2) {
        set_error(out, "unexpected extra arguments");
    }

    return out;
}

} // namespace truecontext::cli
