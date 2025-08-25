#!/bin/sh
set -xe
mkdir -p build
gcc -Wall -Wextra -o build/main src/main.c -I libs/raylib/src -L libs/raylib/src -lraylib -lm
gcc -Wall -Wextra -o build/test src/test.c
