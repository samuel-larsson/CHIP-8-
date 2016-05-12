#include "chip8.h"
#include <iostream>

void chip8::initialize(){
  pc = 0x200;   //PC starts at 0x200
  opcode = 0;   //Reset current opcode
  I = 0;        //Reset index reg.
  sp = 0;       //Reset stack pointer

  //Clear memory
  for(int i = 0; i <= 4096; i++){
    memory[i] = 0;
  }
  //Clear registers V0-VF
  for(int i = 0; i <= 16; i++){
    V[i] = 0;
  }
  //Clear stack
  for(int i = 0; i <= 16; i++){
    stack[i] = 0;
  }
  //Clear display
  for(int i = 0; i <= 64*32; i++){
    gfx[i] = 0;
  }

  //Load fontset
  for(int i = 0; i < 80; ++i){
    memory[i] = chip8_fontset[i];
  }

  //Reset timers
  delay_timer = 0;
  sound_timer = 0;

}

void chip8::emulateCycle(){
  //Fetch opcode
  opcode = memory[pc] << 8 | memory[pc + 1];    // XX00 || 00YY == XXYY

  //Decode opcode
  switch(opcode & 0xF000){
    case 0x0000:
      switch(opcode & 0x000F){
        case 0x0000:  //0x00E0: Clears the screen
        //EXECUTE
        break;
        case 0x000E:  //0x00EE: Returns from a subroutine
        //EXECUTE
        break;

        default:
          std::cout << "Unknown opcode [0x0000]: 0x" << opcode << "\n";
      }
      break;

    case 0x1000:    //0x1NNN Jump to location NNN
      pc = opcode & 0x0FFF;
      break;

    case 0x2000:    //0x2NNN Call subroutine at NNN
      stack[sp] = pc;
      sp++;
      pc = opcode & 0x0FFF;
      break;

    case 0x3000:    //0x3XNN  Skips next instruction if VX == NN
      if(V[opcode & 0x0F00] == (opcode & 0x00FF)){
        pc += 4;
      }
      else {
        pc += 2;
      }
      break;

    case 0x4000:    //0x4XNN Skips next instruction if VX != NN
      if(V[opcode & 0x0F00] != (opcode & 0x00FF)){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

      //CONTINUE HERE
    case 0xA000:    //0xANNN: Sets I to the address NNN
      I = opcode & 0x0FFF;
      pc += 2;
      break;

    default:
      std::cout << "Unknown opcode: 0x" << opcode << "\n";
  }

  //Update timers
  if(delay_timer > 0)
    --delay_timer;

  if(sound_timer > 0){
    if(sound_timer == 1){
      std::cout << "BEEP\n";
      --sound_timer;
    }
  }
}
