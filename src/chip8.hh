#pragma once

#include <fstream>
#include <cstdint>
#include <memory>
#include <random>

static constexpr unsigned int REGISTER_COUNT = 16;
static constexpr unsigned int MEMORY_SIZE = 4096;
static constexpr unsigned int STACK_LEVEL = 16;

static constexpr unsigned int DISPLAY_WIDTH = 64;
static constexpr unsigned int DISPLAY_HEIGHT = 32;

static constexpr unsigned int KEY_COUNT = 16;

class Chip8 {
    public:
        Chip8();
        void load_rom(char const* filename);
        void emulate_cycle();
        
        uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{};

        uint8_t keypad[KEY_COUNT]{};
        bool await_key_flag = false;
        bool await_key_register{};    
    private:
        uint8_t memory[MEMORY_SIZE]{};
        uint8_t V[REGISTER_COUNT]{};
        uint16_t I{};
        uint16_t pc{};

        uint16_t stack[STACK_LEVEL]{};
        uint16_t sp;      

        uint8_t delay_timer{};
        uint8_t sound_timer{};

        std::default_random_engine rng;
        std::uniform_int_distribution<uint8_t> rand_byte;
	
        using OpcodeHandler = void(Chip8::*)(uint16_t);
        OpcodeHandler opcode_table[16];
        /*
	    Subtables for opcodes which have a common leading nibble.
	    These are sized depending on the amount of unique trailing bits.
	    Potential opcodes are 00**, 8XY*, EX**, FX** where * is a unique trailing bit.
        */
        OpcodeHandler opcode_st0[0xF + 1];
        OpcodeHandler opcode_st8[0xF + 1];
        OpcodeHandler opcode_stE[0xF + 1];
        OpcodeHandler opcode_stF[0xFF + 1];

	// refer to https://en.wikipedia.org/wiki/CHIP-8#Opcode_table for what each opcode does
        void opcode0_lookup(uint16_t opcode);
        void opcode00E0(uint16_t opcode);
        void opcode00EE(uint16_t opcode);
	
        void opcode1nnn(uint16_t opcode);
        void opcode2nnn(uint16_t opcode);
        void opcode3xkk(uint16_t opcode);
        void opcode4xkk(uint16_t opcode);
        void opcode5xy0(uint16_t opcode);
        void opcode6xkk(uint16_t opcode);
        void opcode7xkk(uint16_t opcode);
	
        void opcode8_lookup(uint16_t opcode);
        void opcode8xy0(uint16_t opcode);	
        void opcode8xy1(uint16_t opcode);
        void opcode8xy2(uint16_t opcode);
        void opcode8xy3(uint16_t opcode);
        void opcode8xy4(uint16_t opcode);
        void opcode8xy5(uint16_t opcode);
        void opcode8xy6(uint16_t opcode);
        void opcode8xy7(uint16_t opcode);
        void opcode8xyE(uint16_t opcode);

        void opcode9xy0(uint16_t opcode);
        void opcodeAnnn(uint16_t opcode);
        void opcodeBnnn(uint16_t opcode);
        void opcodeCxkk(uint16_t opcode);
        void opcodeDxyn(uint16_t opcode);
        void opcodeE_lookup(uint16_t opcode);
        void opcodeEx9E(uint16_t opcode);
        void opcodeExA1(uint16_t opcode);

        void opcodeF_lookup(uint16_t opcode);
        void opcodeFx07(uint16_t opcode);
        void opcodeFx0A(uint16_t opcode);
        void opcodeFx15(uint16_t opcode);
        void opcodeFx18(uint16_t opcode);
        void opcodeFx1E(uint16_t opcode);
        void opcodeFx29(uint16_t opcode);
        void opcodeFx33(uint16_t opcode);
        void opcodeFx55(uint16_t opcode);
        void opcodeFx65(uint16_t opcode);
};
