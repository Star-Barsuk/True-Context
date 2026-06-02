.PHONY: clean format lint check-format test
.PHONY: config-debug config-release config-sanitize config-lto
.PHONY: build-debug build-release build-sanitize build-lto
.PHONY: debug release sanitize lto
.PHONY: package-linux-x86_64 package-linux-aarch64 package-linux

SRC_FILES := $(shell find src include -type f \( -name "*.cpp" -o -name "*.hpp" \))

# ── Configure (switch preset) ───────────────────────────────

config-debug:
	cmake --preset debug
	@ln -sf build/debug/compile_commands.json compile_commands.json

config-release:
	cmake --preset release

config-sanitize:
	cmake --preset debug-sanitize
	@ln -sf build/debug-sanitize/compile_commands.json compile_commands.json

config-lto:
	cmake --preset release-lto

# ── Build (preset must already be configured) ───────────────

build-debug:
	cmake --build --preset debug

build-release:
	cmake --build --preset release

build-sanitize:
	cmake --build --preset debug-sanitize

build-lto:
	cmake --build --preset release-lto

# ── Configure + build ───────────────────────────────────────

debug: config-debug build-debug

release: config-release build-release

sanitize: config-sanitize build-sanitize

lto: config-lto build-lto

# ── Other ───────────────────────────────────────────────────

format:
	clang-format -i $(SRC_FILES)

check-format:
	clang-format --dry-run --Werror $(SRC_FILES)

lint: check-format

test: build-debug
	ctest --test-dir build/debug --output-on-failure

clean:
	rm -rf build bin compile_commands.json dist

# ── Release packages (for GitHub Releases) ──────────────────
# Native x86_64: run on amd64 Linux.
# arm64: cross-build from x86_64 (see README) or run `make package-linux-aarch64` on arm64 host.

DIST_DIR := dist
RELEASE_BIN := bin/Release/true_context
TOOLCHAIN_AARCH64 := $(CURDIR)/cmake/toolchain-aarch64-linux-gnu.cmake

package-linux-x86_64: release
	@mkdir -p $(DIST_DIR)
	cp $(RELEASE_BIN) $(DIST_DIR)/true_context-linux-x86_64
	strip $(DIST_DIR)/true_context-linux-x86_64
	@echo "→ $(DIST_DIR)/true_context-linux-x86_64"

UNAME_M := $(shell uname -m)

config-release-aarch64:
	cmake --preset release -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_AARCH64)

build-release-aarch64:
	cmake --build --preset release

ifeq ($(UNAME_M),aarch64)
package-linux-aarch64: release
	@mkdir -p $(DIST_DIR)
	cp $(RELEASE_BIN) $(DIST_DIR)/true_context-linux-aarch64
	strip $(DIST_DIR)/true_context-linux-aarch64
	@echo "→ $(DIST_DIR)/true_context-linux-aarch64 (native arm64)"
else
package-linux-aarch64: config-release-aarch64 build-release-aarch64
	@mkdir -p $(DIST_DIR)
	cp $(RELEASE_BIN) $(DIST_DIR)/true_context-linux-aarch64
	aarch64-linux-gnu-strip $(DIST_DIR)/true_context-linux-aarch64
	@echo "→ $(DIST_DIR)/true_context-linux-aarch64 (cross from $(UNAME_M))"
endif

package-linux: package-linux-x86_64 package-linux-aarch64
