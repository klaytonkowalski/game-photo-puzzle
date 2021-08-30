#include "raylib.h"

#include <stdio.h>

static const int screenWidth = 960;
static const int screenHeight = 540;
static const int targetFPS = 60;

void Initialize();
void Update();
void Draw();
void Terminate();

int main (int argc, char *argv[])
{
    Initialize();
    while (!WindowShouldClose())
    {
        Update();
        Draw();
    }
    Terminate();
    return 0;
}

void Initialize()
{
    InitWindow(screenWidth, screenHeight, "Photo Puzzle");
    SetTargetFPS(targetFPS);
}

void Update()
{

}

void Draw()
{

}

void Terminate()
{
    CloseWindow();
}