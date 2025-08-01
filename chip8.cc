#include "chip8.hh"
#include <iostream>

static constexpr unsigned int START_ADDRESS = 0x200;

void Chip8::LoadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(size);

    file.read(reinterpret_cast<char*>(buffer.get()), size);

    for(size_t i = 0; i < static_cast<size_t>(size); ++i) {
	memory[START_ADDRESS + i] = buffer[i];
    }
    
    printf("Loaded ROM with size of %lukb.\n", size);
}
