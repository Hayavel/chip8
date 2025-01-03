

#ifndef CHIP8_H
#define CHIP8_H

struct Ram
{
    unsigned short opcode;
    unsigned char memory[4096];
    
    unsigned short stack[16];
    unsigned short sp;
};

struct CPU
{
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
};

struct Keybord
{
    unsigned char key[16];
};

struct Timer
{
    unsigned char delay_timer;
    unsigned char sound_timer;
};

struct Display
{
    bool videoarray [2048]; // 64*32 - Screen Size
};

class Chip8
{
private:
    Ram ram;
    CPU cpu;
    Keyboard keyboard;
    Timer timer;
    Display display;
public:
    bool drawFlag = false;

    void initialize();
    void loadGame(const char* game);
    void emulateCycle();
    void setKeys();
};

#endif