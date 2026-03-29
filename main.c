#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 4096
#define FONT_SIZE 80
#define WIDTH 64
#define HEIGHT 32

uint8_t font[FONT_SIZE] = {
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

void jump(uint16_t addr);
void call(uint16_t addr);
void ske(uint16_t addr);
void skne(uint16_t addr);
void skre(uint16_t addr);
void set_reg_val(uint16_t addr);
void add_reg_val(uint16_t addr);
void set_reg(uint16_t addr);
void or_reg(uint16_t addr);
void and_reg(uint16_t addr);
void xor_reg(uint16_t addr);
void add_reg(uint16_t addr);
void sub_reg(uint16_t addr);
void shift_right_reg(uint16_t addr);
void sub_assign_reg(uint16_t addr);
void shift_left_reg(uint16_t addr);
void clear_screen();

uint16_t I;
uint16_t pc = 0x200;
uint8_t V[16];
uint8_t memory[MEMORY_SIZE];
uint16_t subroutines[16];
int sp = 0; // stack pointer


int main(int argc, char** argv)
{

    if(argc <= 1) {
        printf("yo gimme a rom");
        return -1;
    }

    char* file_name = argv[1];
    FILE* file = fopen(file_name, "rb");
    if(!file) {
        printf("this shi not a rom");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(&memory[0x200], 1, size, file);
    fclose(file);

    for(size_t i = 0; i < FONT_SIZE; i++) {
        memory[0x50 + i] = font[i]; // common to read font in 0x50
    }

    uint16_t opcode = memory[pc] << 8 | memory[pc+1]; // shift the first instruction 8 bits to the left to add the second instruction so it can become one 16-bit block

    // Source for opcodes: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table

    switch (opcode & 0xF000) // Binary AND with 0xF000 because it masks off the first 4 bytes of any instruction
                             // For example let opcode=0x6A05. 0x6A05 & 0xF000 = 0110101000000101 & 1111000000000000 = 0110 = 0x6.               
    {
    case 0x0:
        if(opcode & 0x00FF == 0xE0) {
            clear_screen();
        } else if (opcode & 0x00FF == 0xEE) {
            pc = subroutines[sp];
            sp--;
        }
        else printf("False instruction");
    case 0x1:
        jump(opcode & 0x0FFF);
        break;
    case 0x2:
        call(opcode & 0x0FFF);
        break;
    case 0x3:
        ske(opcode & 0x0FFF); // skip if equal
        break;
    case 0x4:
        skne(opcode & 0x0FFF); // skip if not equal
        break;
    case 0x5:
        skre(opcode & 0x0FFF); // skip if registers are equal
        break;
    case 0x6:
        set_reg_val(opcode & 0x0FFF); // set register value
        break;
    case 0x7:
        add_reg_val(opcode & 0x0FFF);
        break;
    case 0x8:
        switch (opcode & 0x000F)
        {
        case 0x0:
            set_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x1:
            or_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x2:
            and_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x3:
            xor_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x4:
            add_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x5:
            sub_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x6:
            shift_right_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0x7:
            sub_asign_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0xE:
            shift_left_reg(opcode & 0x0FFF); // assign a register to another
            break;
        }
    case 0xA:
        set_I(opcode & 0x0FFF);
    }

    return 0;
}

void jump(uint16_t addr)
{
    pc = addr;
}

void call(uint16_t addr)
{
    subroutines[sp] = pc;
    sp++;
    pc = addr;
}

void ske(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t NN = addr & 0x0FF;
    if(V[X] == NN)
        pc+=4;
    else pc+=2;
}

void skne(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t NN = addr & 0x0FF;
    if(V[X] != NN)
        pc+=4;
    else pc+=2;
}

void skre(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint8_t Y = addr & 0x0F0;
    if(V[X] != V[Y])
        pc+=4;
    else pc+=2;
}

void set_reg_val(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t NN = addr & 0x0FF;
    V[X] = NN;
}

void add_reg_val(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t NN = addr & 0x0FF;
    V[X] += NN;
}

void set_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] = V[Y];
}

void or_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] |= V[Y];
}

void and_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] &= V[Y];
}

void xor_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] ^= V[Y];
}

void add_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] += V[Y];
}

void sub_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] -= V[Y];
}

void shift_right_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    V[X]>>=1;
}

void sub_assign_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    uint16_t Y = addr & 0x0F0;
    V[X] = V[Y] - V[X];
}

void shift_left_reg(uint16_t addr)
{
    uint8_t X = addr & 0xF00;
    V[X]<<=1;
}

void clear_screen()
{
}