gcc -c -g -Wall -O0 -o obj/chrono.o chrono.c
gcc -c -g -std=c99 -Wall -O0 -o obj/main_copy.o main_copy.c

cd obj

gcc -o ../main.elf main_copy.o chrono.o -lSDL -lm 
