all: main clean

#Link commands
main: main.o chip8functions.o sdl_functions.o
	g++ -o main main.o chip8functions.o sdl_functions.o -I/sw/include/SDL2 -I/usr/X11R6/include -D_THREAD_SAFE -L/sw/lib -lSDL2

#Compilation commands
main.o: main.cpp chip8.h sdl/sdl_functions.cpp
	g++ -c main.cpp
chip8functions.o: chip8functions.cpp chip8.h
	g++ -c chip8functions.cpp
sdl_functions.o:	sdl/sdl_functions.cpp chip8.h
	g++ -c sdl/sdl_functions.cpp
clean:
	rm *.o
