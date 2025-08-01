#pragma once

#include <fstream>
#include <cstdint>
#include <memory>

static constexpr unsigned int REGISTER_COUNT = 16;
static constexpr unsigned int MEMORY_SIZE = 4096;
static constexpr unsigned int STACK_LEVEL = 16;

class Chip8 {
    public:
        void LoadROM(char const* filename);
    private:
        uint16_t opcode{};
        uint8_t memory[MEMORY_SIZE]{};
        uint8_t registers[REGISTER_COUNT]{};
        uint16_t index_register{};
        uint16_t pc{};

        uint8_t stack[STACK_LEVEL]{};
        uint16_t sp;       
}; 
