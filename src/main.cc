#include <memory>
#include "chip8.hh"
#include "graphics.hh"

// converting our Chip-8 display data to something readable by SDL
void c8buff_to_rgba(const uint8_t* c8buff, uint32_t* out_buff, int width, int height) {
    for(int i = 0; i < width * height; i++) {
        out_buff[i] = c8buff[i] ? 0xFFFFFFFF : 0x000000FF;
    }
}

int main(int argc, char** argv) {
    Chip8 chip8;
    Graphics gfx;

    uint32_t sdl_display[DISPLAY_WIDTH * DISPLAY_HEIGHT];

    const char* filename = argv[1];
    chip8.load_rom(filename);
    
    int pitch = sizeof(uint32_t) * DISPLAY_WIDTH;

    bool running = true;
    while(running) {
        gfx.handle_input(chip8.keypad, running);
        chip8.emulate_cycle();

        c8buff_to_rgba(chip8.display, sdl_display, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        gfx.render(sdl_display, pitch);
    }
}
