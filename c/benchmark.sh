#!/usr/bin/env bash

set -euo pipefail

COUNT=100

set +u
if [[ -n "$1" ]]; then
  COUNT="$1"

  if [[ ! "$COUNT" =~ ^[0-9]+$ ]]; then
    echo "Usage" >&2
    exit 1
  fi
else
  echo "nay"
fi
set -u

DIR="$(dirname "$(realpath ${BASH_SOURCE[0]} )" )"
cd "$DIR"

make client server

function run_clients() {
  for ((i = 0 ; i < "$COUNT" ; i++ )); do
    ./client &
  done
}

time ./server --benchmark "$COUNT" &
SERVER_PID=$!

run_clients

wait $SERVER_PID
