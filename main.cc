#include <memory>
#include "chip8.hh"

int main(int argc, char** argv) {
    Chip8 chip8;

    const char* filename = argv[1];
    chip8.load_rom(filename);

    bool running = true;
    while(running) {
	chip8.emulate_cycle();
    }
}
