#include "chip8.hh"
#include <iostream>
#include <cstring>

static constexpr unsigned int START_ADDRESS = 0x200;

Chip8::Chip8() {
    pc = START_ADDRESS;
    std::memset(memory, 0, sizeof(memory));

    opcode_table[0x0] = &Chip8::opcode0xxx;
    opcode_table[0x1] = &Chip8::opcode1nnn;
    opcode_table[0x2] = &Chip8::opcode2nnn;
    opcode_table[0x3] = &Chip8::opcode3xnn;
    opcode_table[0x4] = &Chip8::opcode4xnn;
    opcode_table[0x5] = &Chip8::opcode5xy0;
    opcode_table[0x6] = &Chip8::opcode6xnn;
    opcode_table[0x7] = &Chip8::opcode7xnn;
    opcode_table[0x8] = &Chip8::opcode8xyx;
    opcode_table[0x9] = &Chip8::opcode9xy0;
    opcode_table[0xA] = &Chip8::opcodeAnnn;
    opcode_table[0xB] = &Chip8::opcodeBnnn;
    opcode_table[0xC] = &Chip8::opcodeCxnn;
    opcode_table[0xD] = &Chip8::opcodeDxyn;
    opcode_table[0xE] = &Chip8::opcodeExxx;
    opcode_table[0xF] = &Chip8::opcodeFxxx;
   
    opcode_st0[0xE0] = &Chip8::opcode00E0;
    opcode_st0[0xEE] = &Chip8::opcode00EE;

    opcode_st8[0x0] = &Chip8::opcode8xy0;
    opcode_st8[0x1] = &Chip8::opcode8xy1;
    opcode_st8[0x2] = &Chip8::opcode8xy2;
    opcode_st8[0x3] = &Chip8::opcode8xy3;
    opcode_st8[0x4] = &Chip8::opcode8xy4;
    opcode_st8[0x5] = &Chip8::opcode8xy5;
    opcode_st8[0x6] = &Chip8::opcode8xy6;
    opcode_st8[0x7] = &Chip8::opcode8xy7;
    opcode_st8[0xE] = &Chip8::opcode8xyE;

    opcode_stE[0x9E] = &Chip8::opcodeEx9E;
    opcode_stE[0xA1] = &Chip8::opcodeExA1;

    opcode_stF[0x07] = &Chip8::opcodeFx07;
    opcode_stF[0x0A] = &Chip8::opcodeFx0A;
    opcode_stF[0x15] = &Chip8::opcodeFx15;
    opcode_stF[0x18] = &Chip8::opcodeFx18;
    opcode_stF[0x1E] = &Chip8::opcodeFx1E;
    opcode_stF[0x29] = &Chip8::opcodeFx29;
    opcode_stF[0x33] = &Chip8::opcodeFx33;
    opcode_stF[0x55] = &Chip8::opcodeFx55;
    opcode_stF[0x65] = &Chip8::opcodeFx65;
}

void Chip8::LoadROM(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::unique_ptr<uint8_t[]> buffer = std::make_unique<uint8_t[]>(size);
    file.read(reinterpret_cast<char*>(buffer.get()), size);

    for(size_t i = 0; i < static_cast<size_t>(size); ++i) {
	memory[START_ADDRESS + i] = buffer[i];
    }
    
    printf("Loaded ROM with size of %luB.\n", size);
}

void Chip8::EmulateCycle() {
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    printf("Current opcode: %u\n", (unsigned int)opcode);    
    printf("Current memory: %u\n", (unsigned int)memory[pc]);	
    pc += 2;
    
    uint16_t index = (opcode & 0xF000) >> 12; 

    if(opcode_table[index]) { 
	(this->*opcode_table[index])(opcode); 
    } else { 
	throw std::runtime_error("Unhandled opcode, exiting program."); 
    }

    // check first nibble, if 0, 8, e, f, move to subtable, else check table?

}

void Chip8::opcode0xxx(uint16_t opcode) {
    uint16_t trailing_id = (opcode & 0x00FF) << 8;

    // 0x00E0	0000 0000 1110 0000
    // 0xE0	1110 0000 0000 0000

    if(opcode_st0[trailing_id]) {
	(this->*opcode_st0[trailing_id](opcode);
    } else {
	// unhandled machine code routine call
    }
}

void Chip8::opcode00E0(uint16_t opcode) {
    std::memset(video, 0, sizeof(video));
}

void Chip8::opcode00EE(uint16_t opcode) {
    --sp;
    pc = stack[sp];
}

void Chip8::opcode1nnn(uint16_t opcode) {
    pc = opcode & 0x0FFF;
}   

void Chip8::opcode2nnn(uint16_t opcode) {
    pc = stack[sp++];
    pc = opcode & 0x0FFF;
}

void Chip8::opcode3xnn(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    if(V[x] == byte) {
	pc += 2;
    }
}

void Chip8::opcode4xnn(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    if(V[x] != byte) {
	pc += 2;
    }
}

void Chip8::opcode5xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0 >> 4;
    
    if(V[x] == V[y]) {
	pc += 2;    
    }
}
void Chip8::opcode6xnn(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    V[x] = byte;
}
void Chip8::opcode7xnn(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    V[x] += byte;
}
void Chip8::opcode8xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[y]
}
void Chip8::opcode8xy1(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x000F) >> 4;

    V[x] |= V[y];
}

void Chip8::opcode8xy2(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] &= V[y];
}

void Chip8::opcode8xy3(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] ^= [y];
}
void Chip8::opcode8xy4(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint16_t sum = V[x] + V[y];
    V[0xF] = (sum > 255) ? 1 : 0;
    V[x] = sum & 0xFF;
}

void Chip8::opcode8xy5(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
    V[x] = V[x] - V[y];
}
void Chip8::opcode8xy6(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = V[x] & 0b0001;
    V[x] >>= 1;
}
void Chip8::opcode8xy7(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x000F) >> 4;

    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
    v[x] = V[y] - V[x];
}

void Chip8::opcode8xyE(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = V[x] & 0b0001;
    V[x] <<= 1;
}

void Chip8::opcode9xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[x] != V[y]) {
	pc += 2;
    }
}

void Chip8::opcodeAnnn(uint16_t opcode) {
    index = opcode & 0x0FFF
}
void Chip8::opcodeBnnn(uint16_t opcode) {
    pc = V[0x0] + (opcode & 0x0FFF);
}
void Chip8::opcodeCxnn(uint16_t opcode) {
    
}
void Chip8::opcodeDxyn(uint16_t opcode) {

}
void Chip8::opcodeEx9E(uint16_t opcode) {

}
void Chip8::opcodeExA1(uint16_t opcode) {

}
void Chip8::opcodeFx07(uint16_t opcode) {

}
void Chip8::opcodeFx0A(uint16_t opcode) {

}
void Chip8::opcodeFx15(uint16_t opcode) {

}
void Chip8::opcodeFx18(uint16_t opcode) {

}
void Chip8::opcodeFx1E(uint16_t opcode) {

}
void Chip8::opcodeFx29(uint16_t opcode) {

}
void Chip8::opcodeFx33(uint16_t opcode) {

}
void Chip8::opcodeFx55(uint16_t opcode) {

}
void Chip8::opcodeFx65(uint16_t opcode) {

}
