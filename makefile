#Link commands
main: main.o chip8functions.o
	g++ -o main main.o chip8functions.o

#Compilation commands
main.o: main.cpp chip8.h
	g++ -c main.cpp
chip8functions.o: chip8functions.cpp chip8.h
	g++ -c chip8functions.cpp

clean: rm *.o
