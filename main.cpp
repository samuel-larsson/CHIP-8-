#include "chip8.h"
#include <iostream>
chip8 CHIP8;

int main(){
  std::cout << "Hello! This is the start of the show.\n";

  CHIP8.initialize();

  FILE *gameFile;
  gameFile = std::fopen("Pong [Paul Vervalin, 1990].ch8", "rb");
  if(!gameFile){
    std::perror("File opening failed");
    return EXIT_FAILURE;
  }

  int bufferSize = 3584;
  char buffer[bufferSize];
  setbuf(gameFile, buffer);
  fflush(gameFile);

  for(int i = 0; i < bufferSize; i++){
    CHIP8.memory[512 + i] = buffer[i];
  }

}
