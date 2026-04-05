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
void set_I(uint16_t addr);
void jump_plus(uint16_t addr);
void rand_reg(uint16_t addr);
void draw(uint16_t addr);
void clear_screen();

uint16_t I;
uint16_t pc = 0x200;
uint8_t video[WIDTH][HEIGHT] = {0};
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
    srand(time(NULL));

    for(size_t i = 0; i < FONT_SIZE; i++) {
        memory[0x50 + i] = font[i]; // common to read font in 0x50
    }

    uint16_t opcode = memory[pc] << 8 | memory[pc+1]; // shift the first instruction 8 bits to the left to add the second instruction so it can become one 16-bit block

    // Source for opcodes: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table

    switch (opcode & 0xF000) // Binary AND with 0xF000 because it masks off the first 4 bytes of any instruction
                             // For example let opcode=0x6A05. 0x6A05 & 0xF000 = 0110101000000101 & 1111000000000000 = 0110 = 0x6.
    {
    case 0x0:
        if((opcode & 0x00FF) == 0xE0) {
            clear_screen();
        } else if ((opcode & 0x00FF) == 0xEE) {
            pc = subroutines[sp];
            sp--;
        }
        else printf("Invalid instruction");
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
            sub_assign_reg(opcode & 0x0FFF); // assign a register to another
            break;
        case 0xE:
            shift_left_reg(opcode & 0x0FFF); // assign a register to another
            break;
        }
    case 0xA:
        set_I(opcode & 0x0FFF);
        break;
    case 0xB:
        jump_plus(opcode & 0x0FFF);
        break;
    case 0xC:
        rand_reg(opcode & 0x0FFF);
        break;
    case 0xD:
        draw(opcode & 0x0FFF);
        break;
    case 0xE:
        uint8_t X = ((opcode & 0x0FFF) >> 8) & 0xF;
        if((opcode & 0x00FF) == 0x9E) {
            if(key_pressed(V[X]))
                pc+=4;
            else pc+=2;
        } else if((opcode & 0x00FF) == 0xA1) {
            if(!key_pressed(V[X]))
                pc+=4;
            else pc+=2;
        } else printf("Invalid instruction");
        break;
    case 0xF:
        uint8_t X = ((opcode & 0x0FFF) >> 8) & 0xF;
        switch (opcode & 0x00FF)
        {
            case 0x07:
                V[X] = get_delay();
                pc+=2;
                break;
            case 0x0A:
                V[X] = get_key();
                pc+=2;
                break;
            case 0x15:
                set_delay_timer(V[X]);
                pc+=2;
                break;
            case 0x18:
                set_sound_timer(V[X]);
                pc+=2;
                break;
            case 0x1E:
                I += V[X];
                if(I > 0xFFF)
                    V[0xF] = 1;
                else V[0xF] = 0;
                pc+=2;
                break;
            case 0x29:
                I = 0x50 + V[X] * 5; // V[X] * 5 because each character is 5 bytes tall
                pc+=2;
                break;
            case 0x33:
                uint8_t dig_1 = (uint8_t)(V[X] / 100);
                uint8_t dig_2 = (uint8_t)(V[X] % 100 / 10);
                uint8_t dig_3 = V[X] % 10;
                memory[I] = dig_1;
                memory[I+1] = dig_2;
                memory[I+2] = dig_3;
                pc+=2;
                break;
        }
        break;
    default:
        printf("Invalid instruction");
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
    uint8_t X = (addr >> 8) & 0xF;
    uint16_t NN = addr & 0x0FF;
    if(V[X] == NN)
        pc+=4;
    else pc+=2;
}

void skne(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint16_t NN = addr & 0x0FF;
    if(V[X] != NN)
        pc+=4;
    else pc+=2;
}

void skre(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    if(V[X] != V[Y])
        pc+=4;
    else pc+=2;
}

void set_reg_val(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint16_t NN = addr & 0x0FF;
    V[X] = NN;
    pc+=2;
}

void add_reg_val(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint16_t NN = addr & 0x0FF;
    V[X] += NN;
    pc+=2;
}

void set_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] = V[Y];
    pc+=2;
}

void or_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] |= V[Y];
    pc+=2;
}

void and_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] &= V[Y];
    pc+=2;
}

void xor_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] ^= V[Y];
    pc+=2;
}

void add_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] += V[Y];
    pc+=2;
}

void sub_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] -= V[Y];
    pc+=2;
}

void shift_right_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    V[X]>>=1;
    pc+=2;
}

void sub_assign_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    V[X] = V[Y] - V[X];
    pc+=2;
}

void shift_left_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    V[X]<<=1;
    pc+=2;
}

void set_I(uint16_t addr)
{
    I = addr;
    pc+=2;
}

void jump_plus(uint16_t addr)
{
    pc = addr + V[0];
}

void rand_reg(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint16_t NN = addr & 0x0FF;
    V[X] = rand() & NN;
    pc+=2;
}

void draw(uint16_t addr)
{
    uint8_t X = (addr >> 8) & 0xF;
    uint8_t Y = (addr >> 4) & 0xF;
    uint8_t height = addr & 0x00F;
    uint8_t x_pos = V[X] % WIDTH; // for wraping
    uint8_t y_pos = V[Y] % HEIGHT;

    V[0xF] = 0;
    for(size_t i = 0; i < height; i++) {
        uint8_t sprite_data = memory[I + i];

        for(size_t j = 0; j < 8; j++) {
            uint8_t sprite_pixel = (sprite_data >> j) & 0x1;
            uint8_t* screen_pixel = &video[(x_pos + j) % WIDTH][(y_pos + i) % HEIGHT];

            if(sprite_pixel) {
                if(*screen_pixel)
                    V[0xF] = 1;
                
                *screen_pixel ^= 0xFF;
            }
        }
    }

    //draw(x_pos, y_pos, height);
    pc += 2;
}

void clear_screen()
{
    for(size_t i = 0; i < WIDTH; i++) {
        for(size_t j = 0; j < HEIGHT; j++)
            video[i][j] = 0;
    }

    pc+=2;
}