#!/usr/bin/env bash

ROOT="$(dirname "$(realpath "${BASH_SOURCE[0]}" )" )"
cd "$ROOT/build"

exec bear --append --output "$ROOT/compile_commands.json" -- mk $@
