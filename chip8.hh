#pragma once

#include <fstream>
#include <cstdint>
#include <memory>

static constexpr unsigned int REGISTER_COUNT = 16;
static constexpr unsigned int MEMORY_SIZE = 4096;
static constexpr unsigned int STACK_LEVEL = 16;

static constexpr unsigned int DISPLAY_WIDTH = 64;
static constexpr unsigned int DISPLAY_HEIGHT = 32;

class Chip8 {
    public:
	Chip8();
        void LoadROM(char const* filename);
	void EmulateCycle();
	uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    private:
        uint8_t memory[MEMORY_SIZE]{};
        uint8_t V[REGISTER_COUNT]{};
        uint16_t I{};
        uint16_t pc{};

        uint8_t stack[STACK_LEVEL]{};
        uint16_t sp;      
	 
	using OpcodeHandler = void(Chip8::*)(uint16_t);
	OpcodeHandler opcode_table[16];
	/*
	    Subtables for opcodes which have a common leading nibble.
	    These are sized depending on the amount of unique trailing bits.
	    Potential opcodes are 00**, 8XY*, EX**, FX** where * is a unique trailing bit.
	*/
	OpcodeHandler opcode_st0[0xFF + 1];
	OpcodeHandler opcode_st8[0xF + 1];
	OpcodeHandler opcode_stE[0xFF + 1];
	OpcodeHandler opcode_stF[0xFF + 1];

	// Refer to https://en.wikipedia.org/wiki/CHIP-8#Opcode_table for what each opcode does.	
	// 0x0 opcodes
	void opcode0xxx(uint16_t opcode);
	void opcode00E0(uint16_t opcode);
	void opcode00EE(uint16_t opcode);
	
	// 0x1 - 0x7 opcodes
	void opcode1nnn(uint16_t opcode);
	void opcode2nnn(uint16_t opcode);
	void opcode3xnn(uint16_t opcode);
	void opcode4xnn(uint16_t opcode);
	void opcode5xy0(uint16_t opcode);
	void opcode6xnn(uint16_t opcode);
	void opcode7xnn(uint16_t opcode);
	
	// 0x8 opcodes
	void opcode8xyx(uint16_t opcode);

	void opcode8xy0(uint16_t opcode);	
	void opcode8xy1(uint16_t opcode);
	void opcode8xy2(uint16_t opcode);
	void opcode8xy3(uint16_t opcode);
	void opcode8xy4(uint16_t opcode);
	void opcode8xy5(uint16_t opcode);
	void opcode8xy6(uint16_t opcode);
	void opcode8xy7(uint16_t opcode);
	void opcode8xyE(uint16_t opcode);

	// 0x9 - 0xD
	void opcode9xy0(uint16_t opcode);
	void opcodeAnnn(uint16_t opcode);
	void opcodeBnnn(uint16_t opcode);
	void opcodeCxnn(uint16_t opcode);
	void opcodeDxyn(uint16_t opcode);

	// 0xE	
	void opcodeExxx(uint16_t opcode);

	void opcodeEx9E(uint16_t opcode);
	void opcodeExA1(uint16_t opcode);

	// 0xF
	void opcodeFxxx(uint16_t opcode);

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
