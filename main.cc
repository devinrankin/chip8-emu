#include <memory>
#include "chip8.hh"


int main(int argc, char** argv) {
    Chip8 chip8;

    const char* filename = argv[1];
    chip8.LoadROM(filename);
}
