#!/bin/sh

set -xe

gcc -O2 -o main main.c -I libs/raylib/src -L libs/raylib/src -lraylib -lgdi32 -lwinmm
gcc -o test test.c
