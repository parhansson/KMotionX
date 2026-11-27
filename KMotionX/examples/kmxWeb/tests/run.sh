#!/bin/sh
set -eu
test_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
tmp=$(mktemp -d "${TMPDIR:-/tmp}/kmx-machine-tests.XXXXXX")
trap 'rm -rf "$tmp"' EXIT HUP INT TERM
"${CXX:-c++}" -std=c++20 -Wall -Wextra "$test_dir/machine_paths.cpp" -o "$tmp/test"
"$tmp/test" "$tmp"
