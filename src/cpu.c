#include "functions.h"
#define secondNibbleMask 0b0000111100000000
#define thirdNibbleMask  0b0000000011110000
#define fourthNibbleMask 0b0000000000001111
#define secondByteMask   0b0000000011111111
u_int16_t vI = 536;
u_int16_t stack[16];
u_int8_t SP = 0;
unsigned char tmp;
u_int8_t registers[15];
int DT = 0;
int ST = 0;
int skipInstructionIf(bool check);
int cpuStep(u_int16_t cI){
  int secondNibble = (cI & secondNibbleMask) >> 8;
  int thirdNibble  = (cI & thirdNibbleMask) >> 4;
  int fourthNibble = (cI & fourthNibbleMask);
  int byte2 = cI & secondByteMask;
  switch (cI & 0b1111000000000000) {
    case 0x1000:
    return (cI - 0x1000) - PC;
    case 0x6000:
    registers[secondNibble] = byte2;
    return 2;
    case 0x7000:
    registers[secondNibble] = byte2 + registers[secondNibble];
    return 2;
    case 0xA000:
    vI = ((cI >> 8) & 0x0F) << 8 | (cI & 0xFF); 
    return 2;
    case 0xD000:
    for(int i = 0; i < fourthNibble; i++){
      tmp = memory[vI + i];
      for(int k = 0; k < 8; k++){
        screen[registers[secondNibble] + k][registers[thirdNibble] + i] ^= tmp >> (7-k) & 1;
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
    case 0x8000:
    	u_int8_t * vX = &registers[secondNibble];
	u_int8_t * vY = &registers[thirdNibble];
    	switch (fourthNibble){
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
		*vX += *vY;
		if(*vX + *vY > 255){
		printf("Invert!: %d ", registers[0x0F]);
		registers[0x0F] = !registers[0x0F];
		printf("Postinvert!: %d\n", registers[0x0F]);
		}
		return 2;
		case 5:
		*vX -= *vY;
		if(*vX > *vY){
		registers[0x0F] = !registers[0x0F];
		}
		return 2;
		case 6:
		*vX = *vX >> 1;
		if(*vX & 1 == 1){
		registers[0x0F] = !registers[0x0F];
		}
		return 2;
		case 7:
		*vX = *vY - *vX;
		if(*vY > *vX){
		registers[0x0F] = !registers[0x0F];
		}
		return 2;
		case 0x0E:
		*vX = *vX << 1;
		if(*vX & 128){
		registers[0x0F] = !registers[0x0F];
		}
		return 2;
	    }
    case 0xF000:
	vX = &registers[secondNibble];
	switch(byte2){
	case 0x65:
	for(int i = 0; i < secondNibble + 1; i++){
		registers[i] = memory[i + vI]; 
	}
	return 2;
	case 0x55:
	for(int i = 0; i < secondNibble + 1; i++){
		memory[i + vI] = registers[i]; 
	}
	return 2;
	case 0x33:
	memory[vI] = *vX/100 % 10;
	memory[vI + 1] = *vX/10 % 10;
	memory[vI + 2] = *vX % 10;
	return 2;
	case 0x1E:
	vI = *vX + vI;
	return 2;
	}
	case 0x15:
	ST = *vX;	
	case 0x18:
	DT = *vX;
    case 0xE000:
	vX = &registers[secondNibble];
	switch(byte2){
	case 0x9E:
	skipInstructionIf(keyboard[*vX] == true);
	case 0xA1:
	skipInstructionIf(keyboard[*vX] == false);
	}
    default:
	switch(cI){
	case 0x00E0:
	   for(int i=0; i == 63; i++){
       	     for(int k=0; k == 61; k++){
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
      	//return 2;
    }
  }
}

int skipInstructionIf(bool check){
  if(check){
    return 4;
  }
  else{
    return 2;
  }
}
