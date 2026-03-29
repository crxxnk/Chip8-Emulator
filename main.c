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

void jump();
void call();
void ske();
void skne();
void skie();
void set_reg_val();
void add_reg_val();
void set_reg();
void or_reg();
void and_reg();
void xor_reg();
void add_reg();
void sub_reg();
void shift_right_reg();
void sub_assign_reg();
void shift_left_reg();

int main(int argc, char** argv)
{
    uint16_t I;
    uint16_t pc;
    uint8_t V[16];
    uint8_t memory[MEMORY_SIZE];

    if(argc <= 1) {
        printf("cock");
        return -1;
    }

    char* file_name = argv[1];
    FILE* file = fopen(file_name, "rb");
    if(!file) {
        printf("cock2");
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

    switch (opcode & 0xF000) // Binary AND with 0xF000 because it masks off the first 4 bytes of any instruction
                             // For example let opcode=0x6A05. 0x6A05 & 0xF000 = 0110101000000101 & 1111000000000000 = 0110 = 0x6.               
    {
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
        skie(opcode & 0x0FFF); // skip if instructions are equal
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