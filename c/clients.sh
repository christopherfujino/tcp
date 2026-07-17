#!/usr/bin/env bash

COUNT=10

DIR="$(dirname "$(realpath ${BASH_SOURCE[0]} )" )"
cd "$DIR"

for ((i = 0 ; i < "$COUNT" ; i++ )); do
  ./client &
done

exit
