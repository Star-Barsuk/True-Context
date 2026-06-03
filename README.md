<div align="center">

# True-Context

[![C++23](https://img.shields.io/badge/C++-23-00599C?style=flat&logo=cplusplus&logoColor=white)](https://isocpp.org/) [![CMake](https://img.shields.io/badge/CMake-3.28+-brightgreen?style=flat&logo=cmake&logoColor=white)](https://cmake.org/) [![Ninja](https://img.shields.io/badge/Ninja-Build-orange.svg)](https://ninja-build.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey?style=flat&logo=linux&logoColor=white)](https://github.com/Star-Barsuk/TrueContext)

</div>

Collect project sources into one **markdown** file for LLM context. Git-independent: rules come from **`.contextignore`**.

---

## ⚙️ What it does

- Walks files from a root path
- Applies **`.contextignore`** — globs, `dir/`, `**`, and `!` negation
- Skips binary files and very large files (`MAX_FILE_SIZE`, default 500000 bytes)
- Writes **`docs/context.md`** by default: hierarchical tree + file contents

```bash
true-context .                      # → docs/context.md
true-context src/ docs/review.md    # custom root and output
true-context --help
```

**Always ignored by the tool (built-in):** `.contextignore`, `docs/context.md`.
All other paths (`.git`, `build/`, `node_modules/`, …) — add them to your `.contextignore`.
Copy the template: [`templates/.contextignore`](templates/.contextignore).

---

## 🔧 Build from source

**Requirements:**
| Component | Version |
|-----------|---------|
| CMake | 3.28+ |
| Ninja | Latest |
| GCC | 14+ |
| Clang | 18+ |
| GNU Make | 4.0+ |

```bash
git clone https://github.com/Star-Barsuk/TrueContext.git ./true-context
cd true-context

make debug          # configure + build → bin/Debug/true-context
make release        # optimized → bin/Release/true-context
make test           # ctest smoke tests
```

Build only (after configure): `make build-debug`.

### Linux release binaries (x86_64 & arm64)

| Host CPU | Command | Artifact |
|----------|---------|----------|
| x86_64 (amd64) | `make package-linux-x86_64` | `dist/true-context-linux-x86_64` |
| arm64 (aarch64) | `make package-linux-aarch64` | `dist/true-context-linux-aarch64` |

Build both targets:: `make package-linux` (x86_64 native + arm64 cross on amd64 host).

**Cross-compile arm64 on x86_64 (Debian/Ubuntu)**

Install the ARM64 cross-compilation toolchain:
```bash
sudo apt install \
  gcc-aarch64-linux-gnu \
  g++-aarch64-linux-gnu \
  binutils-aarch64-linux-gnu
```

Build the ARM64 release binary:

```bash
make package-linux-aarch64
```

Verify the generated binary:

```bash
file dist/true-context-linux-aarch64
# ELF 64-bit LSB executable, ARM aarch64, ...
```

Uses [`cmake/toolchain-aarch64-linux-gnu.cmake`](cmake/toolchain-aarch64-linux-gnu.cmake).

**Manual (same as Make targets)**

These commands are equivalent to the Makefile package targets.

```bash
# Linux x86_64 (native)
make release
mkdir -p dist

cp bin/Release/true-context \
  dist/true-context-linux-x86_64
strip dist/true-context-linux-x86_64

# Linux arm64 (cross)
cmake --preset release-aarch64
cmake --build --preset release-aarch64
mkdir -p dist

cp bin/Release/true-context \
  dist/true-context-linux-aarch64
aarch64-linux-gnu-strip \
  dist/true-context-linux-aarch64
```

---

## 🔧 Install from a GitHub release

Pick the asset for your OS on the [Releases](https://github.com/Star-Barsuk/TrueContext/releases) page, or use:

**Linux x86_64**

```bash
curl -fL -o true-context \
  https://github.com/Star-Barsuk/TrueContext/releases/latest/download/true-context-linux-x86_64
chmod +x true-context
sudo mv true-context /usr/local/bin/
```

**Linux arm64**

```bash
curl -fL -o true-context \
  https://github.com/Star-Barsuk/TrueContext/releases/latest/download/true-context-linux-aarch64
chmod +x true-context
sudo mv true-context /usr/local/bin/
```

**Verify**

```bash
true-context --version
true-context /path/to/your/project
```

---

# 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

<div align="center">

**© 2026 Star-Barsuk**

</div>
