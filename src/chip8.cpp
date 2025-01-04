#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
Chip8::Chip8()
{

}
Chip8::~Chip8()
{

}

void Chip8::init()
{
    cpu.pc		= 0x200;		// Program counter starts at 0x200 (Start adress program)
	ram.opcode	= 0;			// Reset current opcode	
	cpu.I		= 0;			// Reset index register
	ram.sp		= 0;			// Reset stack pointer

	// Clear display
	for(size_t i = 0; i < 2048; i++)
		display.videoarray[i] = 0;

	// Clear stack
	for(size_t i = 0; i < 16; i++)
		ram.stack[i] = 0;

	for(size_t i = 0; i < 16; i++)
		keyboard.key[i] = cpu.V[i] = 0;

	// Clear memory
	for(size_t i = 0; i < 4096; i++)
		ram.memory[i] = 0;
					
	// Load fontset
	for(size_t i = 0; i < 80; i++)
		ram.memory[i] = chip8_fontset[i];		

	// Reset timers
	timer.delay_timer = 0;
	timer.sound_timer = 0;

	// Clear screen once
	drawFlag = true;
    std::srand(std::time(nullptr));
}
void Chip8::loadFontSet()
{
    // Load fontset
    for (size_t i=0; i < 80; i++)
        ram.memory[i] = chip8_fontset[i];
}
bool Chip8::loadProgram(const char* program)
{
	init();
	printf("Loading: %s\n", program);
		
	// Open file
	FILE * pFile = fopen(program, "rb");
	if (pFile == NULL)
	{
		fputs ("File error", stderr); 
		return false;
	}

	// Check file size
	fseek(pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);
	printf("Filesize: %d\n", (int)lSize);
	
	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	if (buffer == NULL) 
	{
		fputs ("Memory error", stderr); 
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread (buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		fputs("Reading error",stderr); 
		return false;
	}

	// Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(int i = 0; i < lSize; ++i)
			ram.memory[i + 512] = buffer[i];
	}
	else
		printf("Error: ROM too big for memory");
	
	// Close file, free buffer
	fclose(pFile);
	free(buffer);

	return true;
}
void Chip8::emulateCycle()
{
    // Fetch code
    ram.opcode = (ram.memory[cpu.pc] << 8) | ram.memory[cpu.pc + 1];

    // Decode opcode
    switch(ram.opcode & 0xF000)
    {
        case 0x0000:
        {
            switch(ram.opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears the screen
                {
                    for(int i = 0; i < 2048; i++)
		                display.videoarray[i] = 0x0;
                    drawFlag = true;
                    cpu.pc += 2;
                    break;
                }
                case 0x000E: // 0x00EE: Returns from subroutine
                {
                    --ram.sp;
                    cpu.pc = ram.stack[ram.sp];
                    cpu.pc += 2;
                    break;
                }
                default:
                {
                    std::cout << "Unknown opcode [0x0000]: 0x" << ram.opcode << "\n";
                }
            }
            break;
        }
        // 0NNN: Calls machine code routine at address NNN. Not necessary for most ROMs
        case 0x1000: // 1NNN: Jumps to address NNN
        {
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
            if (cpu.V[(ram.opcode & 0x0F00) >> 8] == cpu.V[(ram.opcode & 0x00F0) >> 4])
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
                    cpu.V[(ram.opcode & 0x0F00) >> 8] |= cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0002: // 8XY2: Sets VX to VX and VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] &= cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0003: // 8XY3: Sets VX to VX xor VY
                {
                    cpu.V[(ram.opcode & 0x0F00) >> 8] ^= cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.pc += 2;
                    break;
                }
                case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's an overflow, and to 0 when there is not
                {
                    int sum = cpu.V[(ram.opcode & 0x0F00) >> 8] + cpu.V[(ram.opcode & 0x00F0) >> 4];
                    cpu.V[0xF] = sum > 0xFF ? 1 : 0; // carry

                    cpu.V[(ram.opcode & 0x0F00) >> 8] = sum & 0x00FF;
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
                    cpu.V[0xF] = cpu.V[(ram.opcode & 0x0F00) >> 8] & 0x1;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] >>= 1;
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
                    cpu.V[0xF] = (cpu.V[(ram.opcode & 0x0F00) >> 8] & 0x80) >> 7;
                    cpu.V[(ram.opcode & 0x0F00) >> 8] <<= 1;
                    cpu.pc += 2;
                    break;
                }
                default:
                {
                    std::cout << "Unknown opcode [0x8000]: 0x" << ram.opcode << "\n";
                }
            }
            break;
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
            cpu.pc = (ram.opcode & 0x0FFF) + cpu.V[0];
            break;
        }
        case 0xC000: // CXkk: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and kk
        {
            cpu.V[(ram.opcode & 0x0F00) >> 8] = (std::rand()%0xFF) & (ram.opcode & 0x00FF);
            cpu.pc += 2;
            break;
        }
        case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
        {
            unsigned short x = cpu.V[(ram.opcode & 0x0F00) >> 8];
            unsigned short y = cpu.V[(ram.opcode & 0x00F0) >> 4];
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
                        if(display.videoarray[(x + xline + ((y + yline) * 64))] == 1)
						{
							cpu.V[0xF] = 1;                              
						}
						display.videoarray[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            cpu.pc += 2;
            break;
        }
        case 0xE000:
        {
            switch (ram.opcode & 0x00FF)
            {
                case 0x009E: // EX9E: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is pressed (usually the next instruction is a jump to skip a code block)
                {
                    if (keyboard.key[cpu.V[(ram.opcode & 0x0F00) >> 8]] != 0)
                        cpu.pc += 2;
                    cpu.pc += 2;
                    break;
                }
                case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX(only consider the lowest nibble) is not pressed (usually the next instruction is a jump to skip a code block)
                {
                    if (keyboard.key[cpu.V[(ram.opcode & 0x0F00) >> 8]] == 0)
                        cpu.pc += 2;
                    cpu.pc += 2;
                    break;
                }
                default:
                {
                    std::cout << "Unknown opcode[0xE000]: 0x" << ram.opcode << "\n";
                }
            }
            break;
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
                    bool keyPress = false;
                    for (size_t i=0; i < 16; i++)
                    {
                        if (keyboard.key[i] != 0)
                        {
                            cpu.V[(ram.opcode & 0x0F00) >> 8] = i;
                            keyPress = true;
                        }
                    }
                    if(!keyPress)
                        return;
                    cpu.pc += 2;
                    break;
                }
                case 0x0015: // FX15: Sets the delay timer to VX
                {
                    timer.delay_timer = cpu.V[(ram.opcode & 0x0F00) >> 8];
                    cpu.pc += 2;
                    break;
                }
                case 0x0018: // FX18: Sets the sound timer to VX
                {
                    timer.sound_timer = cpu.V[(ram.opcode & 0x0F00) >> 8];
                    cpu.pc += 2;
                    break;
                }
                case 0x001E: // FX1E: Adds VX to I. VF is not affected
                {
                    if ((cpu.I + cpu.V[(ram.opcode & 0x0F00) >> 8]) > 0xFFF)
                        cpu.V[0xF] = 1;
                    else
                        cpu.V[0xF] = 0;

                    cpu.I += cpu.V[(ram.opcode & 0x0F00) >> 8];
                    cpu.pc += 2;
                    break;
                }
                case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX(only consider the lowest nibble). Characters 0-F (in hexadecimal) are represented by a 4x5 font
                {
                    cpu.I = cpu.V[(ram.opcode & 0x0F00) >> 8] * 0x5;
                    cpu.pc += 2;
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
                    for (size_t i=0; i <= ((ram.opcode & 0x0F00) >> 8); i++)
                    {
                        ram.memory[cpu.I+i] = cpu.V[i];
                    }
                    // cpu.I += ((ram.opcode & 0x0F00) >> 8) + 1;
                    cpu.pc += 2;
                    break;
                }
                case 0x0065: // FX65: Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified
                {
                    for (size_t i=0; i <= ((ram.opcode & 0x0F00) >> 8); i++)
                    {
                        cpu.V[i] = ram.memory[cpu.I+i];
                    }
                    // cpu.I += ((ram.opcode & 0x0F00) >> 8) + 1;
                    cpu.pc += 2;
                    break;
                }
                default:
                {
                    std::cout << "Unknown opcode[0xF000]: 0x" << ram.opcode << "\n";
                }
            }
            break;
        }
        default:
            std::cout << "Unknown opcode: 0x" << ram.opcode << "\n";
    }
}
void Chip8::updateTimers()
{
    if(timer.delay_timer > 0)
        --timer.delay_timer;
    
    if(timer.sound_timer > 0)
    {
        --timer.sound_timer;
        // return sound;
    }
}
void Chip8::debugRender()
{
	// Draw
	for(int y = 0; y < 32; ++y)
	{
		for(int x = 0; x < 64; ++x)
		{
			if(display.videoarray[(y*64) + x] == 0) 
				std::cout << "O";
			else 
				std::cout << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}