#!/usr/bin/env bash

ROOT="$(dirname "$(realpath ${BASH_SOURCE[0]} )" )"
cd "$ROOT"

COUNT=50

./make.sh build/server build/client

function main() {
  valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --track-fds=no \
    --enable-debuginfod=no \
    --malloc-fill=FF \
    --free-fill=FF \
    ./build/server --benchmark "$COUNT" &
  SERVER_PID=$!

  for ((i=0; i < "$COUNT"; i++)); do
    ./build/client &
  done

  wait $SERVER_PID
}

main 2>&1 | tee ./valgrind.log
