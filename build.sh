#!/bin/sh
set -xe
mkdir -p build
gcc -O2 -o build/main src/main.c -I libs/raylib/src -L libs/raylib/src -lraylib -lm
gcc -o build/test src/test.c
