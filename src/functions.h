#include <stdbool.h> 
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#define pixelSize 16
extern u_int16_t PC;
extern bool screen[64][32];
extern int DT;
extern int ST;
extern bool keyboard[16];
int cpuStep(u_int16_t cI);
void init(char *file);
extern u_int8_t memory[4096];
extern u_int8_t registers[16];
void createLargePixel(int x, int y, int pixel_size, SDL_Renderer *renderer);
