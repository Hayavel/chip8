#include "chip8.h"


void Chip8::loadFontSet()
{
    // Load fontset
    for (size_t i=0; i < 80; ++i)
        ram.memory[i] = chip8_fontset[i];
}
bool Chip8::loadProgram(const char* program)
{
    try
    {
        std::ifstream file(program, std::ios::binary);
        char* buffer;
        size_t bufferSize;
        file.read(buffer, bufferSize);
        for (size_t i=0; i < bufferSize; ++i)
            ram.memory[512 + i] = buffer[i];
        return 1;
    }
    catch (...)
    {
        return 0;
    }

}
char* Chip8::emulateCycle()
{
    // Fetch code
    ram.opcode = ram.memory[cpu.pc] << 8 | ram.memory[cpu.pc + 1];

    // Decode opcode
    switch(ram.opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(ram.opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears the screen
                {
                    std::fill(std::begin(display.videoarray), std::end(display.videoarray), 0);
                    break;
                }
                case 0x000E: // 0x00EE: Returns from subroutine
                {
                    cpu.pc = ram.stack[ram.sp];
                    --ram.sp;
                    break;
                }
                default:
                {
                    return ("Unknown opcode [0x0000]: 0x%X\n", ram.opcode);
                }
            }
        }
        // 0NNN: Calls machine code routine at address NNN. Not necessary for most ROMs
        case 0x1000: // 1NNN: Jumps to address NNN
        {
            ram.stack[ram.sp] = cpu.pc;
            ++ram.sp;
            cpu.pc = ram.opcode & 0x0FFF;
            break;
        }
        case 0x2000: // 2NNN: Calls subroutine at NNN
        {
            ram.stack[ram.sp] = cpu.pc;
            ++ram.sp;
            cpu.pc = ram.opcode & 0x0FFF;
            break;
        }
        case 0x3000: // 3Xkk: Skips the next instruction if VX equals kk (usually the next instruction is a jump to skip a code block)
        {
            if (cpu.V[(ram.opcode & 0x0F00) >> 8] == (ram.opcode & 0x00FF))
                cpu.pc += 2;
            cpu.pc += 2;
            break;
        }
        case 0x4000: // 4Xkk: Skips the next instruction if VX does not equal kk (usually the next instruction is a jump to skip a code block)
        {
            if (cpu.V[(ram.opcode & 0x0F00) >> 8] != (ram.opcode & 0x00FF))
                cpu.pc += 2;
            cpu.pc += 2;
            break;
        }
        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY (usually the next instruction is a jump to skip a code block)
        {
            if (cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x00F0) >> 4])
                cpu.pc += 2;
            cpu.pc += 2;
            break;
        }
        case 0x6000: // 6Xkk: Sets VX to kk
        {
            cpu.V[(ram.opcode & 0x0F00) >> 8] = ram.opcode & 0x00FF;
            cpu.pc += 2;
            break;
        }
        case 0x7000: // 7Xkk: Adds kk to VX (carry flag is not changed)
        {
            cpu.V[(ram.opcode & 0x0F00) >> 8] += ram.opcode & 0x00FF;
            cpu.pc += 2;
            break;
        }
        case 0x8000:
        {
            switch (ram.opcode & 0x000F)
            {
                case 0x0000: // 8XY0: Set VX to the value of VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0001: // 8XY1: Sets VX to VX or VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x0F00) >> 8] | cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0002: // 8XY2: Sets VX to VX and VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x0F00) >> 8] & cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0003: // 8XY3: Sets VX to VX xor VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x0F00) >> 8] ^ cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
                {
                    if(cpu.V[(ram.opcode & 0x00F0) >> 4] > (0xFF - cpu.V[(ram.opcode & 0x0F00) >> 8]))
                        cpu.V[0xF] = 1; // carry
                    else
                        cpu.V[0xF] = 0;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] += cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0005: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's an underflow, and 1 when there is not (i.e. VF set to 1 if VX >= VY and 0 if not)
                {
                    if (cpu.V[(ram.opcode & 0x0F00) >> 8] > cpu.V[(ram.opcode & 0x00F0) >> 4])
                        cpu.V[0xF] = 1;
                    else
                        cpu.V[0xF] = 0;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] -= cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0006: // 8XY6: Shifts VX to the right by 1, then stores the least significant bit of VX prior to the shift into VF
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] >>= 1;
                    if (((cpu.V[(ram.opcode & 0x0F00) >> 8]  % 100) % 10) == 1)
                        cpu.V[0xF] = 1;
                    else
                        cpu.V[0xF] = 0;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] /= 2;
                    cpu.pc += 2;
                    break;
                }
                case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's an underflow, and 1 when there is not (i.e. VF set to 1 if VY >= VX)
                {
                    if (cpu.V[(ram.opcode & 0x00F0) >> 4] > cpu.V[(ram.opcode & 0x0F00) >> 8])
                        cpu.V[0xF] = 1;
                    else
                        cpu.V[0xF] = 0;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = cpu.V[(ram.opcode & 0x00F0) >> 4] - cpu.V[(ram.opcode & 0x0F00) >> 8];
                    cpu.pc += 2;
                    break;
                }
                case 0x000E: // 8XYE: Shifts VX to the left by 1, then sets VF to 1 if the most significant bit of VX prior to that shift was set, or to 0 if it was unset
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] <<= 1;
                    if ((cpu.V[(ram.opcode & 0x0F00) >> 8] / 100) == 1)
                        cpu.V[0xF] = 1;
                    else
                        cpu.V[0xF] = 0;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] *= 2;
                    cpu.pc += 2;
                    break;
                }
                default:
                {
                    return ("Unknown opcode [0x8000]: 0x%X\n", ram.opcode);
                }
            }
        }
        case 0x9000: // 9XY0: Skips the next instruction if VX does not equal VY (Usually the next instruction is a jump to skip a code block)
        {
            if (cpu.V[(ram.opcode & 0x0F00) >> 8] != cpu.V[(ram.opcode & 0x00F0) >> 4])
                cpu.pc += 2;
            cpu.pc += 2;
            break;
        }

        case 0xA000: // ANNN: Sets I to the address NNN
        {
            cpu.I = ram.opcode & 0x0FFF;
            cpu.pc += 2;
            break;
        }

        case 0xB000: // BNNN: Jumps to the address NNN plus V0
        {
            ram.stack[ram.sp] = cpu.pc;
            ++ram.sp;
            cpu.pc = (ram.opcode & 0x0FFF) + cpu.V[0];
            break;
        }
        case 0xC000: // CXkk: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and kk
        {
            cpu.V[(ram.opcode & 0x0F00) >> 8] = (std::rand()%255) & (ram.opcode & 0x00FF);
            cpu.pc += 2;
            break;
        }
        case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
        {
            unsigned short x = cpu.V[(ram.opcode & 0x0F00) >> 8];
            unsigned short x = cpu.V[(ram.opcode & 0x00F0) >> 4];
            unsigned short height = ram.opcode & 0x000F;
            unsigned short pixel;

            cpu.V[0xF] = 0;
            for (size_t yline = 0; yline < height; yline++)
            {
                pixel = ram.memory[cpu.I + yline];
                for (size_t xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        if (display.videoarray[(x + xline + ((y + yline) * 64))] == 1)
                            cpu.V[0xF] = 1;
                        display.videoarray[(x + xline + ((y + yline) * 64))] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc += 2;
            break;
        }
        case 0xE000:
        {
            switch (ram.opcode & 0x00FF)
            {
                case 0x009E: // EX9E: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block)
                {
                    if (keyboard.key[cpu.V[(ram.opcode & 0x0F00) >> 8]] != 0)
                        pc += 2;
                    pc += 2;
                    break;
                }
                case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block)
                {
                    if (keyboard.key[cpu.V[(ram.opcode & 0x0F00) >> 8]] == 0)
                        pc += 2;
                    pc += 2;
                    break;
                }
                default:
                {
                    return ("Unknown opcode[0xE000]: 0x%X\n", ram.opcode);
                }
            }
        }
        case 0xF000:
        {
            switch (ram.opcode & 0x00FF)
            {
                case 0x0007: // FX07: Sets VX to the value of the delay timer
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] = timer.delay_timer;
                    cpu.pc += 2;
                    break;
                }
                case 0x000A: // FX0A: A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event, delay and sound timers should continue processing)
                {
                    // unsigned char = 
                    // cpu.V[(ram.opcode & 0x0F00) >> 8] =
                    break;
                }
                case 0x0015: // FX15: Sets the delay timer to VX
                {
                    break;
                }
                case 0x0018: // FX18: Sets the sound timer to VX
                {
                    break;
                }
                case 0x001E: // FX1E: Adds VX to I. VF is not affected
                {
                    break;
                }
                case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
                {
                    break;
                }
                case 0x0033: // FX33: Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
                {
                    ram.memory[cpu.I] = cpu.V[(ram.opcode & 0x0F00) >> 8] / 100;
                    ram.memory[cpu.I+1] = (cpu.V[(ram.opcode & 0x0F00) >> 8] / 10) % 10;
                    ram.memory[cpu.I+2] = (cpu.V[(ram.opcode & 0x0F00) >> 8] % 100) % 10;
                    cpu.pc += 2;
                    break;
                }
                case 0x0055: // FX55: Stores form V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified
                {
                    break;
                }
                case 0x0065: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
                {
                    break;
                }
                default:
                {
                    return ("Unknown opcode[0xF000]: 0x%X\n", ram.opcode);
                }
            }
        }
        default:
            return ("Unknown opcode: 0x%X\n", ram.opcode);
    }
    return "";
}
bool Chip8::updateTimers()
{
    if(timer.delay_timer > 0)
        --timer.delay_timer;
    
    if(timer.sound_timer > 0)
    {
        bool sound = false;
        if(timer.sound_timer == 1)
            sound = true;
        --timer.sound_timer;
        return sound;
    }
}
void Chip8::setKeys()
{

}