#!/usr/bin/env bash

ROOT="$(dirname "$(realpath ${BASH_SOURCE[0]} )" )"
cd "$ROOT"

COUNT=10

make server

function main() {
  valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    ./server --benchmark "$COUNT" &
  SERVER_PID=$!

  for ((i=0; i < "$COUNT"; i++)); do
    ./client &
  done

  wait $SERVER_PID
}

main 2>&1 | tee ./valgrind.log
