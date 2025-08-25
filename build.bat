mkdir -p build
gcc -Wall -Wextra -o build/main src/main.c -I libs/raylib/src -L libs/raylib/src -lraylib -lgdi32 -lwinmm
gcc -Wall -Wextra -o build/test src/test.c 
