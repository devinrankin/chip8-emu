#include "chip8.hh"
#include <iostream>
#include <cstring>
#include <unistd.h>


static constexpr unsigned int START_ADDRESS = 0x200;
static constexpr unsigned int FONTSET_SIZE = 80;
static constexpr unsigned int FONTSET_START_ADDRESS = 0x050;

uint8_t fontset[FONTSET_SIZE] = { 
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

Chip8::Chip8() {
    pc = START_ADDRESS;
    sp = 0;
    std::memset(memory, 0, sizeof(memory));

    for(unsigned int i = 0; i < FONTSET_SIZE; i++) {
	memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    opcode_table[0x0] = &Chip8::opcode0_lookup;
    opcode_table[0x1] = &Chip8::opcode1nnn;
    opcode_table[0x2] = &Chip8::opcode2nnn;
    opcode_table[0x3] = &Chip8::opcode3xkk;
    opcode_table[0x4] = &Chip8::opcode4xkk;
    opcode_table[0x5] = &Chip8::opcode5xy0;
    opcode_table[0x6] = &Chip8::opcode6xkk;
    opcode_table[0x7] = &Chip8::opcode7xkk;
    opcode_table[0x8] = &Chip8::opcode8_lookup;
    opcode_table[0x9] = &Chip8::opcode9xy0;
    opcode_table[0xA] = &Chip8::opcodeAnnn;
    opcode_table[0xB] = &Chip8::opcodeBnnn;
    opcode_table[0xC] = &Chip8::opcodeCxkk;
    opcode_table[0xD] = &Chip8::opcodeDxyn;
    opcode_table[0xE] = &Chip8::opcodeE_lookup;
    opcode_table[0xF] = &Chip8::opcodeF_lookup;
   
    opcode_st0[0x0] = &Chip8::opcode00E0;
    opcode_st0[0xE] = &Chip8::opcode00EE;

    opcode_st8[0x0] = &Chip8::opcode8xy0;
    opcode_st8[0x1] = &Chip8::opcode8xy1;
    opcode_st8[0x2] = &Chip8::opcode8xy2;
    opcode_st8[0x3] = &Chip8::opcode8xy3;
    opcode_st8[0x4] = &Chip8::opcode8xy4;
    opcode_st8[0x5] = &Chip8::opcode8xy5;
    opcode_st8[0x6] = &Chip8::opcode8xy6;
    opcode_st8[0x7] = &Chip8::opcode8xy7;
    opcode_st8[0xE] = &Chip8::opcode8xyE;

    opcode_stE[0xE] = &Chip8::opcodeEx9E;
    opcode_stE[0x1] = &Chip8::opcodeExA1;

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

void Chip8::load_rom(const char* filename) {
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

void Chip8::emulate_cycle() {
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
    
    uint16_t index = (opcode & 0xF000) >> 12; 
    if(opcode_table[index]) { 
	(this->*opcode_table[index])(opcode); 
    } else { 
	throw std::runtime_error("Unhandled opcode, exiting program."); 
    }
}

void Chip8::opcode0_lookup(uint16_t opcode) {
    uint16_t index = (opcode & 0x000F);

    if(opcode_st0[index]) {
	(this->*opcode_st0[index])(opcode);
    } else {
	// unhandled machine code routine call
    }
}

void Chip8::opcode8_lookup(uint16_t opcode) {
    (this->*opcode_st8[opcode & 0xF])(opcode);
}

void Chip8::opcodeE_lookup(uint16_t opcode) { 
    (this->*opcode_stE[opcode & 0xF])(opcode);
}

void Chip8::opcodeF_lookup(uint16_t opcode) {	
    (this->*opcode_stF[opcode & 0xFF])(opcode);
}

// CLS
void Chip8::opcode00E0(uint16_t opcode) {
   std::memset(display, 0, sizeof(display));
}

// RET
void Chip8::opcode00EE(uint16_t opcode) {
    --sp;
    pc = stack[sp];
}

// JP addr
void Chip8::opcode1nnn(uint16_t opcode) {
     pc = opcode & 0x0FFF;
}   

// CALL addr
void Chip8::opcode2nnn(uint16_t opcode) {
    stack[sp++] = pc;
    pc = opcode & 0x0FFF;
}

// SE Vx, byte
void Chip8::opcode3xkk(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    if(V[x] == kk) {
	pc += 2;
    }
}

// SNE Vx, byte
void Chip8::opcode4xkk(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    if(V[x] != kk) {
	pc += 2;
    }
}

// SE Vx, Vy
void Chip8::opcode5xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    
    if(V[x] == V[y]) {
	pc += 2;    
    }
}

// LD Vx, byte
void Chip8::opcode6xkk(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    V[x] = kk;
}

// ADD Vx, byte
void Chip8::opcode7xkk(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;

    V[x] += kk;
}

// LD Vx, Vy
void Chip8::opcode8xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] = V[y];
}

// OR Vx, Vy
void Chip8::opcode8xy1(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x000F) >> 4;

    V[x] |= V[y];
}

// AND Vx, Vy
void Chip8::opcode8xy2(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] &= V[y];
}

// XOR Vx, Vy
void Chip8::opcode8xy3(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[x] ^= V[y];
}

// ADD Vx, Vy
void Chip8::opcode8xy4(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint16_t sum = V[x] + V[y];
    V[0xF] = (sum > 255) ? 1 : 0;
    V[x] = sum & 0xFF;
}

// SUB Vy, Vx
void Chip8::opcode8xy5(uint16_t opcode) { 
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
    V[x] = V[x] - V[y];
}

// SHR Vx
void Chip8::opcode8xy6(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = V[x] & 0b0001;
    V[x] >>= 1;
}

// SUBN Vx, Vy
void Chip8::opcode8xy7(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x000F) >> 4;

    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
    V[x] = V[y] - V[x];
}

// SHL Vx
void Chip8::opcode8xyE(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    V[0xF] = V[x] & 0b0001;
    V[x] <<= 1;
}

// SNE Vx, Vy
void Chip8::opcode9xy0(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    if(V[x] != V[y]) {
	pc += 2;
    }
}

// LD I, addr
void Chip8::opcodeAnnn(uint16_t opcode) {
    I = opcode & 0x0FFF;
}

// JP V0, addr
void Chip8::opcodeBnnn(uint16_t opcode) {
    pc = V[0x0] + (opcode & 0x0FFF);
}
void Chip8::opcodeCxkk(uint16_t opcode) {
}

// DRW Vx, Vy, height
void Chip8::opcodeDxyn(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    uint8_t pos_x = V[x] % DISPLAY_WIDTH;
    uint8_t pos_y = V[y] % DISPLAY_HEIGHT;

    uint8_t height = opcode & 0x000F;

    V[0xF] = 0;

    for(unsigned int row = 0; row < height; ++row) {
        uint8_t spr_byte = memory[I + row];
        for(unsigned int col = 0; col < 8; ++col) {
            if((spr_byte & (0x80 >> col)) == 0) continue;

            int px_x = (pos_x + col) % DISPLAY_WIDTH;
            int px_y = (pos_y + row) % DISPLAY_HEIGHT;

            int idx = px_y * DISPLAY_WIDTH + px_x;
            if(display[idx]) V[0xF] = 1;

            display[idx] ^=1;
        }
    }
}

// SKP Vx
void Chip8::opcodeEx9E(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = V[x];

    if(key < 16 && keypad[key]) {
        pc += 2;
    }
}

// SKNP Vx
void Chip8::opcodeExA1(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = V[x];

    if(key < 16 & !keypad[key]) {
        pc +=2;
    }
}

// LD Vx, DT
void Chip8::opcodeFx07(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    V[x] = delay_timer;
}

// LD Vx, K
void Chip8::opcodeFx0A(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if(!await_key_flag) {
        await_key_flag = true;
        await_key_register = x;
        return;
    }

    for(uint8_t key = 0; key < 16; ++key) {
        if(keypad[key]) {
            V[await_key_register] = key;
            await_key_flag = false;
            pc += 2;
            break;
        }
    }
}

// LD DT, Vx
void Chip8::opcodeFx15(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    delay_timer = V[x];
}

// LD ST, Vx
void Chip8::opcodeFx18(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    sound_timer = V[x];
}

// add Vx, I
void Chip8::opcodeFx1E(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    I += V[x];
}

// LD I, Vx
void Chip8::opcodeFx29(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    I = FONT_START_ADDRESS + (V[x] * 5);
}

// LD B, Vx
void Chip8::opcodeFx33(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t value = V[x];

    memory[I] = value / 100;
    memory[I + 1] = (value / 10) % 10;
    memory[I + 2] = value % 10;
}

// LD [I], Vx
void Chip8::opcodeFx55(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(uint8_t i = 0; i <= x; ++i) {
	memory[I + i] = V[i];
    }
}

// LD Vx, [I]
void Chip8::opcodeFx65(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for(uint8_t i = 0; i <= x; ++i) {
	V[i] = memory[I + i];
    }
}
