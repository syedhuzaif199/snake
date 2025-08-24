mkdir -p build
gcc -O2 -o build/main src/main.c -I libs/raylib/src -L libs/raylib/src -lraylib -lgdi32 -lwinmm
gcc -O2 -o build/test src/test.c 
