#!/usr/bin/env bash

set -euo pipefail

COUNT=20

DIR="$(dirname "$(realpath ${BASH_SOURCE[0]} )" )"
cd "$DIR"

make client server

function run_clients() {
  for ((i = 0 ; i < "$COUNT" ; i++ )); do
    ./client &
  done
}

time ./server --benchmark $COUNT &
run_clients
