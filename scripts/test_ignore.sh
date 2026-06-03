#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT/bin/Debug/true-context"
FIXTURE="$ROOT/tests/fixtures/ignore_sample"
OUT="$FIXTURE/out.md"

if [[ ! -x "$BIN" ]]; then
    echo "error: build true-context first (make debug)" >&2
    exit 1
fi

rm -f "$OUT"
(cd "$FIXTURE" && "$BIN" . out.md)
OUT="$FIXTURE/out.md"

assert_contains() {
    if ! grep -qF "$1" "$OUT"; then
        echo "FAIL: expected in context: $1" >&2
        exit 1
    fi
}

assert_missing() {
    if grep -qF "$1" "$OUT"; then
        echo "FAIL: should not be in context: $1" >&2
        exit 1
    fi
}

assert_contains "tests/golden/keep.txt"
assert_contains "include-me"
assert_contains "README.md"

assert_missing "tests/a.txt"
assert_missing "test_1/hidden.txt"
assert_missing "templates/skip.txt"

echo "OK: ignore + negation fixture"
