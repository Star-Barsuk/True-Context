#include <truecontext/app/application.hpp>
#include <truecontext/app/exit_code.hpp>
#include <truecontext/cli/parser.hpp>
#include <truecontext/cli/usage.hpp>
#include <truecontext/collect/collector.hpp>
#include <truecontext/version.hpp>

namespace truecontext::app {

    Application::Application(int argc, char* argv[]) noexcept
        : argc_{argc},
          argv_{argv} {
    }

    int Application::dispatch(const cli::Options& options) {
        if (!cli::parse_ok(options)) {
            stderr_.write_cstr(truecontext::kName);
            stderr_.write_cstr(": ");
            stderr_.write_line(options.error);
            stderr_.write_line("Try 'true-context --help'.");
            return kExitUsage;
        }

        switch (options.action) {
            case cli::Action::Help:
                cli::Usage::print_help(stdout_);
                return kExitOk;

            case cli::Action::Version:
                cli::Usage::print_version(stdout_);
                return kExitOk;

            case cli::Action::Collect: {
                collect::Collector collector{options};
                return collector.run(stderr_, stdout_);
            }
        }

        return kExitError;
    }

    int Application::run() {
        const cli::Options options = cli::Parser::parse(argc_, argv_);
        return dispatch(options);
    }

} // namespace truecontext::app
