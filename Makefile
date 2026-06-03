.PHONY: clean format lint check-format test
.PHONY: config-debug config-release config-sanitize config-lto
.PHONY: build-debug build-release build-sanitize build-lto
.PHONY: debug release sanitize lto
.PHONY: package-linux-x86_64 package-linux-aarch64 package-linux

SRC_FILES := $(shell find src include -type f \( -name "*.cpp" -o -name "*.hpp" \))
UNAME_M := $(shell uname -m)

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

# ── Release packages ────────────────────────────────────────
DIST_DIR := dist
TOOLCHAIN_AARCH64 := $(CURDIR)/cmake/toolchain-aarch64-linux-gnu.cmake

package-linux-x86_64:
	@mkdir -p $(DIST_DIR)
	cmake --preset release-x86_64
	cmake --build --preset release-x86_64
	cp bin/Release/true_context $(DIST_DIR)/true_context-linux-x86_64
	strip $(DIST_DIR)/true_context-linux-x86_64
	@echo "→ $(DIST_DIR)/true_context-linux-x86_64"

package-linux-aarch64:
	@mkdir -p $(DIST_DIR)
	cmake --preset release-aarch64
	cmake --build --preset release-aarch64
	cp bin/Release/true_context $(DIST_DIR)/true_context-linux-aarch64
ifeq ($(UNAME_M),aarch64)
	strip $(DIST_DIR)/true_context-linux-aarch64
	@echo "→ $(DIST_DIR)/true_context-linux-aarch64 (native arm64)"
else
	aarch64-linux-gnu-strip $(DIST_DIR)/true_context-linux-aarch64
	@echo "→ $(DIST_DIR)/true_context-linux-aarch64 (cross from $(UNAME_M))"
endif

package-linux: package-linux-x86_64 package-linux-aarch64
