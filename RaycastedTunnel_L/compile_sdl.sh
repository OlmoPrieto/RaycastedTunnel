gcc -c -g -Wall -O0 -o obj/chrono.o chrono.c
gcc -c -g -std=c99 -Wall -O0 -o obj/main_sdl.o main_sdl.c
gcc -c -g -Wno-trigraphs -o obj/lava.o lava.c

cd obj

gcc -o ../main_sdl.elf lava.o main_sdl.o chrono.o -lSDL -lm 
