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
          break;
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
      if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
        pc += 4;
      }
      else {
        pc += 2;
      }
      break;

    case 0x4000:    //0x4XNN Skips next instruction if VX != NN
      if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

    case 0x5000:    //0x5XY0 Skips next intr. if VX == VY
      if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

    case 0x6000:    //0x6XNN Loads NN to VX
      V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
      pc += 2;
      break;

    case 0x7000:    //0x7XNN Loads NN+VX to VX
      V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
      pc += 2;
      break;

    case 0x8000:    //0x8XYN has 9 different branches depending on last 4 bits
      switch(opcode & 0x000F){
        case 0x0000: //0x8XY0 Move VY to VX
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        case 0x0001: //0x8XY1 Set VX to (VX OR VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] | V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0002: //0x8XY2 Set VX to (VX AND VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] & V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0003: //0x8XY3 Set VX to (VX XOR VY)
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] ^ V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x0004: //0x8XY4 Set VX = VX + VY, VF = carry
          if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] += V[opcode & 0x00F0 >> 4];
          pc += 2;
          break;

        case 0x0005: //0x8XY5 Set VX = VX - VY, VF = NOT borrow
          if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]){
            V[0xF] = 1;
          }
          else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
          pc += 2;
          break;

        case 0x0006:  //0x8XY6 Set VX = VX SHR 1
          if((V[(opcode & 0x0F00) >> 8] & 1) == 1){
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] / 2;
          pc += 2;
          break;

        case 0x0007:  //0x8XY7 Set VX = VY - VX, VF = NOT borrow
          if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]){
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
          pc += 2;
          break;

        case 0x000E:  //0x8XYE Set VX = VX SHL 1
          if((V[(opcode & 0x0F00) >> 8] & 8) == 1){
            V[0xF] = 1;
          } else {
            V[0xF] = 0;
          }
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] * 2;
          pc += 2;
          break;

        default:
          std::cout << "Unknown opcode [0x8000]: 0x" << opcode << "\n";
          break;
      }
      break;

    case 0x9000:  //0x9XY0 Skip next instruction if VX != VY
      if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
        pc += 4;
      } else {
        pc += 2;
      }
      break;

    case 0xA000:  //0xANNN: Sets I to the address NNN
      I = opcode & 0x0FFF;
      pc += 2;
      break;

    case 0xB000:  //0xBNNN Jump to adress NNN + V0
      pc = (opcode & 0x0FFF) + V[0];
      break;

    case 0xC000:  //0xCXKK Set VX = random byte AND KK
      V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
      pc += 2;
      break;

    case 0xD000:{ //0xDXYN Display N-pixel rows high sprite starting at memory location I at (VX, VY), set VF = collision
      //Get (x,y)-position of sprite
      unsigned short x = V[(opcode & 0x0F00) >> 8];
      unsigned short y = V[(opcode & 0x00F0) >> 4];
      unsigned short height = opcode & 0x000F;
      unsigned short pixel;

      V[0xF] = 0;
      for(int yline = 0; yline < height; yline++){  //loop over each row
        pixel = memory[I + yline];                  //fetch pixel values from memory (1 or 0)
        for(int xline = 0; xline <  8; xline++){
          if((pixel & (0x80 >> xline)) != 0){         //scan through all 8 evaluated pixel, checking if 1 or 0
            if(gfx[x + xline + (y + yline)*64] == 1){
              V[0xF] = 1;
            }
            gfx[x + xline + (y + yline)*64] ^= 1;     //set pixel value
          }
        }
      }
      //drawFlag = true;  //update screen
      pc += 2;
      break;
    }

    case 0xE000:
      switch (opcode & 0x00FF){
        case 0x009E:  //0xEX9E Skip next instr. if key with the value of VX is pressed.
          if(key[V[(opcode & 0x0F00) >> 8]] != 0){
            pc += 4;
          } else {
            pc += 2;
          }
        break;

        case 0x00A1:  //0xEXA1 Skip next instr. if key with the value of VX is NOT pressed.
          if(key[V[(opcode & 0x0F00) >> 8]] == 0){
            pc += 4;
          } else {
            pc += 2;
          }
        break;

        default:
          std::cout << "Unknown opcode [0xE000]: 0x" << opcode << "\n";
          break;
      }
      break;

    case 0xF000:
      switch(opcode & 0x00FF){
        case 0x0007:  //0xFX07 Set VX = delay timer value
          V[(opcode & 0x0F00) >> 8] = delay_timer;
          pc += 2;
          break;

        case 0x000A:  //0xFX0A Wait for a key press, store value of key in VX
          //FINISH LATER
          break;

        case 0x0015:  //0xFX15 Set delay timer = value of VX
          delay_timer = V[(opcode & 0x0F00) >> 8] ;
          break;
        //FINISH ALL THE F-OPCODES LATER

        default:
          std::cout << "Unknown opcode [0xF000]: 0x" << opcode << "\n";
          break;
      }
      break;

    default:
      std::cout << "Unknown opcode: 0x" << opcode << "\n";
      break;
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
