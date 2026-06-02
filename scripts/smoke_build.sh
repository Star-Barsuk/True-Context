#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

echo "==> configure + build (debug)"
make debug

BIN="$ROOT/bin/Debug/true_context"
if [[ ! -x "$BIN" ]]; then
    echo "error: expected executable at $BIN" >&2
    exit 1
fi

echo "==> --help"
"$BIN" --help | head -n 3

echo "==> --version"
"$BIN" --version | head -n 1

echo "==> ctest"
ctest --test-dir build/debug --output-on-failure

echo "OK: smoke_build passed"
