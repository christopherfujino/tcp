#!/usr/bin/env bash

DIR="$(dirname "$(realpath "${BASH_SOURCE[0]}" )" )"
cd "$DIR"

exec bear --append -- make --file .Makefile $@
