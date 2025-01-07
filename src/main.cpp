#include <cstdlib>
#include <ctime>
#include <iostream>

#include <Raylib/raylib.h>

#include "chip8.h"

void drawGraphics();
void keyboardDown();
void keyboardUp();

Chip8 chip8{};

int main(int argc, char* argv[])
{
    InitWindow(1280, 640, "Chip8");
    SetWindowIcon(LoadImage("chip8.png"));
    RenderTexture2D target = LoadRenderTexture(64, 32);
    const float virtualRatio = 64.0f/32.0f;
    Camera2D worldSpaceCamera = {0};  // Game world camera
    worldSpaceCamera.zoom = 20.0f;

    float cameraX = 0.0f;
    float cameraY = 0.0f;

    if (argc > 1)
    {
        if(!chip8.loadProgram(argv[1]))
        {
            std::cout << "Chip8 Load Program Error: Program is not load\n";
            return 4;
        }
    }
    else
    {
        if(!chip8.loadProgram("games/Pong (1 player).ch8"))
        {
            std::cout << "Chip8 Load Pong Error: Pong is not load\n";
            return 4;
        }
    }
    // chip8.loadFontSet();
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        keyboardDown();
        keyboardUp();
        chip8.emulateCycle();
        chip8.updateTimers();
        BeginTextureMode(target);
            if(chip8.drawFlag)
            {
                ClearBackground(BLACK);
                drawGraphics();
                chip8.drawFlag = false;
            }
        EndTextureMode();
        BeginDrawing();
            BeginMode2D(worldSpaceCamera);
                DrawTexture(target.texture, 0, 0, WHITE);
            EndMode2D();
        EndDrawing();
    }

    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}

void drawGraphics()
{
    for (size_t y=0; y < 32; y++)
    {
        for (size_t x=0; x < 64; x++) // x
        {
            if (chip8.display.videoarray[y*64 + x])
                DrawPixel(x, 31-y, WHITE);
        }
    }
}

void keyboardDown()
{
	if(IsKeyDown(KEY_ONE))	    chip8.keyboard.key[0x1] = 1;
	if(IsKeyDown(KEY_TWO))	    chip8.keyboard.key[0x2] = 1;
	if(IsKeyDown(KEY_THREE))	chip8.keyboard.key[0x3] = 1;
	if(IsKeyDown(KEY_FOUR))	    chip8.keyboard.key[0xC] = 1;

	if(IsKeyDown(KEY_Q))	    chip8.keyboard.key[0x4] = 1;
	if(IsKeyDown(KEY_W))	    chip8.keyboard.key[0x5] = 1;
	if(IsKeyDown(KEY_E))	    chip8.keyboard.key[0x6] = 1;
	if(IsKeyDown(KEY_R))	    chip8.keyboard.key[0xD] = 1;

	if(IsKeyDown(KEY_A))	    chip8.keyboard.key[0x7] = 1;
	if(IsKeyDown(KEY_S))	    chip8.keyboard.key[0x8] = 1;
	if(IsKeyDown(KEY_D))	    chip8.keyboard.key[0x9] = 1;
	if(IsKeyDown(KEY_F))	    chip8.keyboard.key[0xE] = 1;

	if(IsKeyDown(KEY_Z))	    chip8.keyboard.key[0xA] = 1;
	if(IsKeyDown(KEY_X))	    chip8.keyboard.key[0x0] = 1;
	if(IsKeyDown(KEY_C))	    chip8.keyboard.key[0xB] = 1;
	if(IsKeyDown(KEY_V))	    chip8.keyboard.key[0xF] = 1;
}

void keyboardUp()
{
	if(IsKeyUp(KEY_ONE))	    chip8.keyboard.key[0x1] = 0;
	if(IsKeyUp(KEY_TWO))	    chip8.keyboard.key[0x2] = 0;
	if(IsKeyUp(KEY_THREE))	    chip8.keyboard.key[0x3] = 0;
	if(IsKeyUp(KEY_FOUR))	    chip8.keyboard.key[0xC] = 0;

	if(IsKeyUp(KEY_Q))	        chip8.keyboard.key[0x4] = 0;
	if(IsKeyUp(KEY_W))	        chip8.keyboard.key[0x5] = 0;
	if(IsKeyUp(KEY_E))	        chip8.keyboard.key[0x6] = 0;
	if(IsKeyUp(KEY_R))	        chip8.keyboard.key[0xD] = 0;

	if(IsKeyUp(KEY_A))	        chip8.keyboard.key[0x7] = 0;
	if(IsKeyUp(KEY_S))	        chip8.keyboard.key[0x8] = 0;
	if(IsKeyUp(KEY_D))	        chip8.keyboard.key[0x9] = 0;
	if(IsKeyUp(KEY_F))	        chip8.keyboard.key[0xE] = 0;

	if(IsKeyUp(KEY_Z))	        chip8.keyboard.key[0xA] = 0;
	if(IsKeyUp(KEY_X))	        chip8.keyboard.key[0x0] = 0;
	if(IsKeyUp(KEY_C))	        chip8.keyboard.key[0xB] = 0;
	if(IsKeyUp(KEY_V))	        chip8.keyboard.key[0xF] = 0;
}