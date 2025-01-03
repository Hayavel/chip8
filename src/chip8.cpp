#include "chip8.h"


void Chip8::initialize()
{
    cpu.pc = 0x200; // Program counter starts at 0x200
    ram.opcode = 0; // Reset current opcode
    cpu.I = 0;      // Reset index register 
    ram.sp = 0;     // Reset stack pointer

    // Clear display
    // Clear stack
    // Clear register V0-VF
    // Clear memory

    // Load fontset
    for (size_t i=0; i < 80; ++i)
        ram.memory[i] = chip8_fontset[i];
    
    // Reset timer
}
void Chip8::loadProgram(const char* program)
{
    std::ifstream file(program, std::ios::binary);
    char* buffer;
    size_t bufferSize;
    file.read(buffer, bufferSize);
    for (size_t i=0; i < bufferSize; ++i)
        ram.memory[512 + i] = buffer[i];
}
char* Chip8::emulateCycle()
{

}
bool Chip8::updateTimers()
{

}
void Chip8::setKeys()
{

}