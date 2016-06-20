#include "../chip8.h"
#include <iostream>

//Declarations


//Definitions
bool video_init(){
  if(SDL_Init(SDL_INIT_VIDEO) != 0){
    std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_Window *win = SDL_CreateWindow("CHIpp-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64, 32, SDL_WINDOW_SHOWN);

  if(win == NULL){
    std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    SDL_Quit;
    return false;
  }
  return true;
}
