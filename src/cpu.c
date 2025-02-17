#include "functions.h"
#define secondNibbleMask 0x0F00
#define thirdNibbleMask 0x00F0
#define fourthNibbleMask 0x000F
#define secondByteMask 0x00FF
u_int16_t vI = 0;
u_int16_t stack[16];
u_int8_t SP = 0;
unsigned char tmp;
u_int8_t registers[16];
int DT = 0;
int ST = 0;
int skipInstructionIf(bool check);
bool keyboardCopy;
bool vF;
int cpuStep(u_int16_t cI) {
  if (ST != 0) {
    ST--;
  }
  if (DT != 0) {
    DT--;
  }
  int secondNibble = (cI & secondNibbleMask) >> 8;
  int thirdNibble = (cI & thirdNibbleMask) >> 4;
  int fourthNibble = (cI & fourthNibbleMask);
  u_int8_t *vX = &registers[secondNibble];
  u_int8_t *vY = &registers[thirdNibble];
  int byte2 = cI & secondByteMask;
  switch (cI & 0xF000) {
  case 0x1000:
    return (cI - 0x1000) - PC;
  case 0x6000:
    registers[secondNibble] = byte2;
    return 2;
  case 0x7000:
    registers[secondNibble] = byte2 + registers[secondNibble];
    return 2;
  case 0xA000:
    vI = cI & 0x0FFF;
    return 2;
  case 0xD000:
    for (int i = 0; i < fourthNibble; i++) {
      tmp = memory[vI + i];
      for (int k = 0; k < 8; k++) {
        vF = screen[registers[secondNibble] + k][registers[thirdNibble] + i];
        screen[registers[secondNibble] + k][registers[thirdNibble] + i] ^=
            tmp >> (7 - k) & 1;
        if (screen[registers[secondNibble] + k][registers[thirdNibble] + i] &&
            !vF) {
          registers[0xF] = 1;
        }
      }
    }
    return 2;
  case 0x3000:
    return skipInstructionIf(registers[secondNibble] == byte2);
  case 0x4000:
    return skipInstructionIf(registers[secondNibble] != byte2);
  case 0x5000:
    return skipInstructionIf(registers[secondNibble] == registers[thirdNibble]);
  case 0x9000:
    return skipInstructionIf(registers[secondNibble] != registers[thirdNibble]);
  case 0x2000:
    SP++;
    stack[SP] = PC;
    PC = cI & 0b0000111111111111;
    return 0;
  case 0xC000:
    registers[secondNibble] = (rand() % 255) && byte2;
    return 2;
  case 0x8000:
    vF = registers[0xF];
    switch (fourthNibble) {
    case 0:
      *vX = *vY;
      return 2;
    case 1:
      *vX = *vX | *vY;
      return 2;
    case 2:
      *vX = *vX & *vY;
      return 2;
    case 3:
      *vX = *vX ^ *vY;
      return 2;
    case 4:
      if (*vX + *vY < 255) {
        vF = !vF;
      }
      *vX += *vY;
      registers[0xF] = vF;
      return 2;
    case 5:
      *vX -= *vY;
      if (*vX > *vY) {
        registers[0x0F] = !registers[0x0F];
      }
      return 2;
    case 6:
      vF = *vX & 1;
      *vX >>= 1;
      registers[0xF] = vF;
      return 2;
    case 7:
      vF = *vY > *vX;
      *vX = *vY - *vX;
      registers[0xF] = vF;
      return 2;
    case 0x0E:
      vF = *vX & 128;
      *vX = *vX << 1;
      registers[0xF] = vF;
      return 2;
    }
  case 0xF000:
    vX = &registers[secondNibble];
    switch (byte2) {
    case 0x65:
      for (int i = 0; i < secondNibble + 1; i++) {
        registers[i] = memory[i + vI];
      }
      return 2;
    case 0x55:
      for (int i = 0; i < secondNibble + 1; i++) {
        memory[i + vI] = registers[i];
      }
      return 2;
    case 0x33:
      memory[vI] = *vX / 100 % 10;
      memory[vI + 1] = *vX / 10 % 10;
      memory[vI + 2] = *vX % 10;
      return 2;
    case 0x1E:
      vI = *vX + vI;
      return 2;
    case 0x15:
      DT = *vX;
      return 2;
    case 0x18:
      ST = *vX;
      return 2;
    case 0x07:
      *vX = DT;
      return 2;
    case 0x29:
      vI = *vX * 5;
      return 2;
    case 0xA:
      keyboardCopy = false;
      for (int i = 0; i < 16; i++) {
        if (keyboard[i]) {
          if (keyboardCopy != keyboard[i]) {
            *vX = i;
            return 2;
          }
          keyboardCopy = keyboard[i];
        }
      }
      return 0;
    }
  case 0xE000:
    vX = &registers[secondNibble];
    switch (byte2) {
    case 0x9E:
      return skipInstructionIf(keyboard[*vX] == true);
    case 0xA1:
      return skipInstructionIf(keyboard[*vX] == false);
    }
  default:
    switch (cI) {
    case 0x00E0:
      for (int i = 0; i < 63; i++) {
        for (int k = 0; k < 61; k++) {
          screen[i][k] = false;
        }
      }
      return 2;
    case 0x00EE:
      PC = stack[SP];
      SP--;
      return 2;
    default:
      char fail[256];
      sprintf(fail, "Unknown Opcode 0x%X", cI);
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", fail, NULL);
      exit(2);
      return 2;
    }
  }
}

int skipInstructionIf(bool check) {
  if (check) {
    return 4;
  } else {
    return 2;
  }
}
