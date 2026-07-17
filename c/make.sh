#!/usr/bin/env bash

if [[ $# -gt 0 ]]; then
  bear -- make --file ./call-bear.Makefile $@
else
  bear -- make --file ./call-bear.Makefile
fi
