.PHONY: clean format lint check-format test
.PHONY: config-debug config-release config-sanitize config-lto
.PHONY: build-debug build-release build-sanitize build-lto
.PHONY: debug release sanitize lto

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
	rm -rf build bin compile_commands.json
