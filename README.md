# true_context

Native C++ port of [`collect_context.sh`](collect_context.sh): collect project sources into `context.txt` for LLM context, with `.contextignore` support.

**Status:** MVP collector — writes [`docs/context.md`](docs/context.md) (markdown). See [`todo.md`](todo.md) for P0 gaps (gitignore negation, hierarchical tree, etc.).

## Requirements

- CMake ≥ 3.28
- Ninja
- GCC or Clang with C++23

```bash
cmake --version
ninja --version
g++ --version   # or clang++
```

## Quick start

```bash
# Configure (Debug) + build
make debug

# Run
./bin/Debug/true_context --help
./bin/Debug/true_context --version
```

## Build variants

| Command | Preset | Notes |
|---------|--------|--------|
| `make debug` | `debug` | Default dev build (`-O0 -g`) |
| `make release` | `release` | Optimized (`-O3`) |
| `make sanitize` | `debug-sanitize` | ASan + UBSan |
| `make lto` | `release-lto` | Release + LTO |

Configure without building: `make config-debug`  
Build only (after configure): `make build-debug`

Outputs land under `bin/<BuildType>/` (e.g. `bin/Debug/true_context`).

## Tests

```bash
make debug
make test
```

Runs CMake `ctest` (smoke: `--help` / `--version`).

Manual smoke script:

```bash
./tests/smoke_build.sh
```

## Formatting

```bash
make format        # apply clang-format
make check-format  # CI-style check
```

## Debugging (VS Code / Cursor)

1. `make debug` once (generates `compile_commands.json` symlink).
2. Open **Run and Debug** → **Debug true_context**.
3. Set breakpoints in `src/main.cpp` (or future modules).

`.vscode/launch.json` and `tasks.json` are committed for this workflow.

CLI debugging without IDE:

```bash
gdb --args ./bin/Debug/true_context --help
```

With sanitizers:

```bash
make sanitize
./build/debug-sanitize/true_context --version   # or bin/Debug if preset links there — use preset binaryDir for ASan runs
```

After `make sanitize`, the binary is under `build/debug-sanitize/`; `bin/Debug/` follows `CMAKE_BUILD_TYPE=Debug` from that configure.

## Project layout

```text
CMakeLists.txt          Root build
CMakePresets.json       Ninja presets
Makefile                Shortcuts (debug, test, format)
src/main.cpp            Entry point → app::Application
include/truecontext/    Headers by layer (cli, io, app, collect, …)
docs/ARCHITECTURE.md    Layers, namespaces, pipeline
collect_context.sh      Reference shell implementation
PROJECT_GOALS.md        Goals, constraints, phases
todo.md                 Feature backlog
tests/                  Smoke scripts + CTest
```

## Collecting context

From the repo root:

```bash
make debug
./bin/Debug/true_context .
# → docs/context.md

# Custom output path
./bin/Debug/true_context . docs/review.md
```

Optional: `MAX_FILE_SIZE=500000` (bytes, default 500000).

Shell reference implementation: [`collect_context.sh`](collect_context.sh) → `context.txt`.

## License

See [LICENSE](LICENSE).
