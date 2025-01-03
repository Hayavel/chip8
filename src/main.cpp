#include <bitset>
#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>

#include "chip8.h"

int main(int argc, char* argv[])
{
    SDL_log
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("SDL_Init Error: %s\n", SDL_GetError())
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("CHIP 8", 100, 100, 64, 32, SDL_WINDOW_SHOWN)
    if (window == nullptr)
    {
        SDL_Log("SDL_CreateWindow Error: %s\n", SDL_GetError())
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
    if (renderer == nullptr)
    {
        SDL_Log("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}