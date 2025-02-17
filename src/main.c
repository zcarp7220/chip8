#include "functions.h"
#define ROM_SIZE 1000
#define FPS 120
#define SAMPLE_RATE 44100
#define AMPLITUDE 28000
bool keyboard[16];
bool screen[64][32];
bool quit = false;
const char keymap[17] = "1234qwerasfzxcv";
char* check;
void callback(void *userdata, Uint8 *stream, int len);
void squareWave(int pitch); 
void audioInit();
char keyinput;
static int freq = 440;  
u_int8_t memory[4096] = {
0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
0x20, 0x60, 0x20, 0x20, 0x70, // 1
0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
0x90, 0x90, 0xF0, 0x10, 0x10, // 4
0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
0xF0, 0x10, 0x20, 0x40, 0x40, // 7
0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
0xF0, 0x90, 0xF0, 0x90, 0x90, // A
0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
0xF0, 0x80, 0x80, 0x80, 0xF0, // C
0xE0, 0x90, 0x90, 0x90, 0xE0, // D
0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
u_int8_t temp[ROM_SIZE];
u_int16_t PC = 0x200;
void init(char *file){
    memset(registers, 0, sizeof(registers));
    memset(keyboard, false, sizeof(keyboard));
    FILE *rom;
    size_t length;
    rom = fopen(file, "r");

    if (rom == NULL) {
        perror("Cannot open");
        exit(-1);
    }

    length = fread(temp, sizeof(u_int8_t), ROM_SIZE, rom);

    if (length != ROM_SIZE) {
        if (ferror(rom)) {
            perror("Error reading file");
            fclose(rom);
            exit(2);
        }
    }

    for (size_t i = 0; i < length; i++) {
        memory[i + 512] = temp[i];
    }
    fclose(rom);
//Temp
//memory[0x1FF] = 1;

  }
int main(int argc, char ** argv){
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    if(argv[1] != NULL){
    init(argv[1]);
    }
    else{
        printf("Please enter a file name");
        exit(1);
    }
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_CreateWindowAndRenderer(pixelSize * 64, pixelSize * 32, 0, &window, &renderer);
    audioInit();
    while (!quit) {
    int m_startTime = SDL_GetTicks();
    SDL_RenderClear(renderer);
    for (int x=0; x < 64; x++){
        for (int y=0; y < 128; y++){
            if(screen[x][y]){
            createLargePixel(x, y, pixelSize, renderer);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderPresent(renderer);
    SDL_PollEvent(&event); 
    switch(event.type){
    case SDL_QUIT:
	   quit = true;
    case SDL_KEYDOWN:
	keyinput = event.key.keysym.sym;
	check = strchr(keymap ,keyinput);
	if (check){
	keyboard[(int)(check - keymap)] = true;
	}
	break;
    case SDL_KEYUP:
	keyinput = event.key.keysym.sym;
	check = strchr(keymap,keyinput);
	if (check){
	keyboard[(int)(check - keymap)] = false;
	}
	break;
    }
    //Run CPU Cycle
    printf("PC: 0x%X| Adress at PC: 0x%X \n", PC, (memory[PC] << 8) | memory[PC + 1]);
    PC = PC + cpuStep((memory[PC] << 8) | memory[PC + 1]);
    //Buzzer
    if(ST != 0){
	squareWave(440);
    }
    else{
	squareWave(1);
    }
    //FPS Limiting
      if (1000 / FPS > SDL_GetTicks() - m_startTime)
        {
            SDL_Delay((1000 / FPS) - (SDL_GetTicks() - m_startTime));
        }
    }
exit:;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
void createLargePixel(int x, int y, int pixel_size, SDL_Renderer *renderer){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = { x * pixel_size, y * pixel_size, pixel_size, pixel_size };
    SDL_RenderFillRect(renderer, &rect);
}
void audio_callback(void* userdata, Uint8* stream, int len) {
    static bool high = true;
    static int sample_count = 0;
    Sint16* buffer = (Sint16*)stream;
    int length = len / 2;  
    for (int i = 0; i < length; ++i) {
        if (sample_count >= SAMPLE_RATE / (freq * 2)) { 
            high = !high;
            sample_count = 0;
        }

        buffer[i] = high ? AMPLITUDE : -AMPLITUDE;
        sample_count++;
    }
}
void audioInit(){
    SDL_AudioSpec desired_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = SAMPLE_RATE;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = 1;
    desired_spec.samples = 128;
    desired_spec.callback = audio_callback;

    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &desired_spec, NULL, 0);
    SDL_PauseAudioDevice(device_id, 0);
    
}
void squareWave(int pitch) {
    freq = pitch;
}
