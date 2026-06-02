# Architecture — true_context

## Layering

```text
main.cpp
    └── truecontext::app::Application
            ├── truecontext::cli::Parser / Usage
            ├── truecontext::io::StdoutWriter / StderrWriter / FileWriter
            └── truecontext::collect::Collector
                    ├── walk::Walker      opendir recursion → PathList
                    ├── ignore::Matcher   built-in self + `.contextignore` (`!` negation)
                    │       └── gitignore_match  `**/`, trailing `/`, globs
                    ├── sort::Sorter      priority sort
                    └── render::Renderer  → docs/context.md (markdown)
                            └── TreeRenderer  hierarchical ├── / └── / │
```

Default output: **`docs/context.md`** (relative to cwd). Override: `true_context . docs/other.md`.

Dependencies flow **inward**: `collect` may use `cli` types; `cli` and `io` do not depend on `collect`.

## Namespaces

| Namespace | Responsibility |
|-----------|----------------|
| `truecontext` | Version constants (`version.hpp`) |
| `truecontext::util` | Header-only helpers (`cstring`, later `path`) |
| `truecontext::io` | POSIX stdout/stderr writers; file I/O later |
| `truecontext::cli` | `Options`, `Parser`, `Usage` |
| `truecontext::app` | `Application`, exit code constants |
| `truecontext::collect` | End-to-end collector (`Collector`) |
| `truecontext::walk` | *(planned)* Directory traversal |
| `truecontext::ignore` | *(planned)* Default + `.contextignore` |
| `truecontext::sort` | *(planned)* Path ordering |
| `truecontext::render` | *(planned)* Tree + file bodies + stats |

## Core types

### `cli::Action`

What to do after parsing: print help, print version, or run collection.

### `cli::Options`

Plain struct — no virtuals, no STL containers:

- `action` — `Help` | `Version` | `Collect`
- `root_path` — pointer into `argv` or `"."` for collect root
- `error` — non-null if parse failed (message for stderr)

### `cli::Parser`

Stateless `parse(argc, argv) → Options`.

### `app::Application`

Constructed with `argc`/`argv`; `run()` dispatches on `Options` and returns a process exit code.

### `collect::Collector`

Owns collection settings (from `Options`); `run()` executes the pipeline. Stub today; will call walk/ignore/sort/render modules.

### `io::StdoutWriter` / `io::StderrWriter`

Thin wrappers over `write(2)` — all user-visible text goes through these (no `<iostream>`).

## Planned pipeline (inside `collect`)

```text
Collector::run()
  1. walk::Walker        → file path list (arena / realloc, no vector)
  2. ignore::Matcher     → filter paths
  3. sort::Sorter        → ordered paths
  4. render::Renderer    → context.txt (tree + contents + stats)
```

## CMake layout

```text
src/
  main.cpp
  io/stdout_writer.cpp
  cli/parser.cpp
  cli/usage.cpp
  app/application.cpp
  collect/collector.cpp
include/truecontext/
  version.hpp
  util/cstring.hpp
  io/...
  cli/...
  app/...
  collect/...
```

Static library `truecontext_core` linked into executable `true_context` — enables future unit tests against the same objects.

## Conventions

- **No** `std::vector`, **no** `<iostream>` in core code
- Prefer `const char*`, `string_view`, fixed buffers (`PATH_MAX`) for paths
- Classes with single responsibility; parsers/renderers are stateless where possible
- Errors: parse errors → stderr + exit `2`; runtime errors → stderr + exit `1`
