#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

cmake -S "$ROOT_DIR" -B "$ROOT_DIR/build"
cmake --build "$ROOT_DIR/build"

make -C "$ROOT_DIR/demo"

echo "running demo..."
"$ROOT_DIR/build/riscv_sim" "$ROOT_DIR/demo/demo.elf"
