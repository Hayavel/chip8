#include "chip8.h"


void Chip8::loadFontSet()
{
    // Load fontset
    for (size_t i=0; i < 80; ++i)
        ram.memory[i] = chip8_fontset[i];
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
    // Fetch code
    ram.opcode = ram.memory[cpu.pc] << 8 | ram.memory[cpu.pc + 1];

    // Decode opcode
    switch(opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears the screen
                {
                    std::fill(std::begin(display.videoarray), std::end(display.videoarray), 0);
                    break;
                }
                case 0x000E: // 0x00EE: Returns from subroutine
                {
                    return;
                    break;
                }
                default:
                {
                    return ("Unknown opcode [0x0000]: 0x%X\n", opcode);
                }
            }
        }

        // 0NNN: Calls machine code routine at address NNN. Not necessary for most ROMs
        case 0x1000: // 1NNN: Jumps to address NNN
        {

            break;
        }
        case 0x2000: // 2NNN: Calls subroutine at NNN
        {
            break;
        }
        case 0x3000: // 3Xkk: Skips the next instruction if VX equals kk (usually the next instruction is a jump to skip a code block)
        {
            break;
        }
        case 0x4000: // 4Xkk: Skips the next instruction if VX does not equal kk (usually the next instruction is a jump to skip a code block)
        {
            break;
        }
        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
        {
            break;
        }
        case 0x6000: // 6Xkk: Sets VX to kk
        {
            break;
        }
        case 0x7000: // 7Xkk: Adds kk to VX (carry flag is not changed)
        {
            break;
        }
        case 0x8000:
        {

        }
        // 8XY0: Set VX to the value of VY
        // 8XY1: Sets VX to VX or VY
        // 8XY2: Sets VX to VX and VY
        // 8XY3: Sets VX to VX xor VY
        // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
        // 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not (i.e. VF set to 1 if VX >= VY and 0 if not)
        // 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
        // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not (i.e. VF set to 1 if VY >= VX)
        // 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
        // 9XY0: Skips the next instruction if VX does not equal VY (Usually the next instruction is a jump to skip a code block)

        case 0xA000: // ANNN: Sets I to the address NNN
        {
            cpu.I = opcode & 0x0FFF;
            cpu.pc += 2;
            break;
        }

        // BNNN: Jumps to the address NNN plus V0
        // CXkk: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and kk
        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
                // Each row of 8 pixels is read as bit-coded starting from memory location I; 
                // I value does not change after the execution of this instruction. As described above, 
                // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
                // and to 0 if that does not happen
        // EX9E: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block)
        // EXA1: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block)
        // FX07: Sets VX to the value of the delay timer
        // FX0A: A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing)
        // FX15: Sets the delay timer to VX
        // FX18: Sets the sound timer to VX
        // FX1E: Adds VX to I. VF is not affected
        // FX29: Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
        // FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
        // FX55: Stores form V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
        // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified

        default:
            return ("Unknown opcode: 0x%X\n", opcode);
    }
}
bool Chip8::updateTimers()
{
    if(timer.delay_timer > 0)
        --timer.delay_timer;
    
    if(timer.sound_timer > 0)
    {
        bool sound = false;
        if(sound_timer == 1)
            sound = true;
        --sound_timer;
        return sound;
    }
}
void Chip8::setKeys()
{

}