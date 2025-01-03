#include <cstdlib>
#include <ctime>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>

#include "chip8.h"

Chip8 chip8;

int main(int argc, char* argv[])
{
    std::srand(std::time(nullptr));

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("CHIP 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return 2;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr)
    {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 3;
    }

    if (argc > 1)
    {
        if(!chip8.loadGame(argv[1]))
        {
            SDL_Log("Chip8 Load Program Error: Program is not load\n");
            return 4;
        }
    }
    else
    {
        if(!chip8.loadGame("games/Pong (1 player).ch8"))
        {
            SDL_Log("Chip8 Load Pong Error: Pong is not load\n");
            return 4;
        }
    }

    while(true)
    {
        chip8.emulateCycle();
        chip8.updateTimers();
        
        if(chip8.drawFlag)
            // drawGraphics();
        chip8.setKeys();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}