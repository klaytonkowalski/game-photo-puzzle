//////////////////////////////////////////////////////////////////////
// LICENSE
//////////////////////////////////////////////////////////////////////

// MIT License

// Copyright (c) 2021 Klayton Kowalski

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// https://github.com/klaytonkowalski/game-photo-puzzle

//////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////

#include "raylib.h"

#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// DEFINES
//////////////////////////////////////////////////////////////////////

#define PHOTO_COUNT 7
#define TILE_COUNT 40

//////////////////////////////////////////////////////////////////////
// CONSTANTS
//////////////////////////////////////////////////////////////////////

static const int photoWidth = 960;
static const int photoHeight = 600;
static const int screenWidth = 960;
static const int screenHeight = 650;
static const int targetFPS = 90;
static const int tileSize = 120;
static const int tileColumns = 8;

//////////////////////////////////////////////////////////////////////
// LOADED PROPERTIES
//////////////////////////////////////////////////////////////////////

static Sound buttonSound;
static Sound selectTileSound;
static Sound swapTileSound;
static Sound completeSound;
static Texture photos[PHOTO_COUNT];
static RenderTexture renderTexture;

//////////////////////////////////////////////////////////////////////
// PROPERTIES
//////////////////////////////////////////////////////////////////////

static int photoIndex;
static int selectedTileIndex;
static int tiles[TILE_COUNT];
static bool clickingNewPuzzle;
static bool clickingSolvePuzzle;
static bool clickingResetPuzzle;

//////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////

static void Initialize();
static void Update();
static void Draw();
static void Terminate();
static void ResetTiles();
static void ScrambleTiles();
static bool IsComplete();

//////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////

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

static void Initialize()
{
    InitWindow(screenWidth, screenHeight, "Photo Puzzle");
    SetTargetFPS(targetFPS);
    InitAudioDevice();
    buttonSound = LoadSound("Assets/Sounds/Button.wav");
    selectTileSound = LoadSound("Assets/Sounds/SelectTile.wav");
    swapTileSound = LoadSound("Assets/Sounds/SwapTile.wav");
    completeSound = LoadSound("Assets/Sounds/Complete.wav");
    photos[0] = LoadTexture("Assets/Photos/Photo1.png");
    photos[1] = LoadTexture("Assets/Photos/Photo2.png");
    photos[2] = LoadTexture("Assets/Photos/Photo3.png");
    photos[3] = LoadTexture("Assets/Photos/Photo4.png");
    photos[4] = LoadTexture("Assets/Photos/Photo5.png");
    photos[5] = LoadTexture("Assets/Photos/Photo6.png");
    photos[6] = LoadTexture("Assets/Photos/Photo7.png");
    photos[7] = LoadTexture("Assets/Photos/Photo8.png");
    photos[8] = LoadTexture("Assets/Photos/Photo9.png");
    renderTexture = LoadRenderTexture(photoWidth, photoHeight);
    photoIndex = 0;
    selectedTileIndex = -1;
    clickingNewPuzzle = false;
    clickingSolvePuzzle = false;
    clickingResetPuzzle = false;
    ScrambleTiles();
}

static void Update()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        const Vector2 mousePosition = GetMousePosition();
        if (CheckCollisionPointRec(mousePosition, (Rectangle) { 10, 10, 150, 30 }))
        {
            clickingNewPuzzle = true;
        }
        else if (CheckCollisionPointRec(mousePosition, (Rectangle) { 170, 10, 150, 30 }))
        {
            clickingSolvePuzzle = true;
        }
        else if (CheckCollisionPointRec(mousePosition, (Rectangle) { 330, 10, 150, 30 }))
        {
            clickingResetPuzzle = true;
        }
        else if (CheckCollisionPointRec(mousePosition, (Rectangle) { 0, 50, photoWidth, photoHeight }))
        {
            if (selectedTileIndex == -1)
            {
                selectedTileIndex = (int) (mousePosition.x / tileSize) + ((int) (mousePosition.y - 50) / tileSize * tileColumns);
                PlaySound(selectTileSound);
            }
            else
            {
                const int swapTileIndex = (int) (mousePosition.x / tileSize) + ((int) (mousePosition.y - 50) / tileSize * tileColumns);
                const int selectedTileValue = tiles[selectedTileIndex];
                tiles[selectedTileIndex] = tiles[swapTileIndex];
                tiles[swapTileIndex] = selectedTileValue;
                selectedTileIndex = -1;
                PlaySound(IsComplete() ? completeSound : swapTileSound);
            }
        }
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        const Vector2 mousePosition = GetMousePosition();
        if (CheckCollisionPointRec(mousePosition, (Rectangle) { 10, 10, 150, 30 }))
        {
            photoIndex = (photoIndex + 1) % PHOTO_COUNT;
            ScrambleTiles();
            PlaySound(buttonSound);
        }
        else if (CheckCollisionPointRec(mousePosition, (Rectangle) { 170, 10, 150, 30 }))
        {
            ResetTiles();
            PlaySound(buttonSound);
        }
        else if (CheckCollisionPointRec(mousePosition, (Rectangle) { 330, 10, 150, 30 }))
        {
            ScrambleTiles();
            PlaySound(buttonSound);
        }
        clickingNewPuzzle = false;
        clickingSolvePuzzle = false;
        clickingResetPuzzle = false;
    }
}

static void Draw()
{
    BeginDrawing();
    BeginTextureMode(renderTexture);
    DrawTexture(photos[photoIndex], 0, 0, WHITE);
    EndTextureMode();
    ClearBackground(WHITE);
    DrawRectangle(0, 0, screenWidth, 50, SKYBLUE);
    DrawRectangle(10, 10, 150, 30, clickingNewPuzzle ? DARKBLUE : BLUE);
    const int newPuzzleWidth = MeasureText("New Puzzle", 20);
    DrawText("New Puzzle", 10 + (150 - newPuzzleWidth) * 0.5, 15, 20, clickingNewPuzzle ? GRAY : WHITE);
    DrawRectangle(170, 10, 150, 30, clickingSolvePuzzle ? DARKBLUE : BLUE);
    const int solvePuzzleWidth = MeasureText("Solve Puzzle", 20);
    DrawText("Solve Puzzle", 170 + (150 - solvePuzzleWidth) * 0.5, 15, 20, clickingSolvePuzzle ? GRAY : WHITE);
    DrawRectangle(330, 10, 150, 30, clickingResetPuzzle ? DARKBLUE : BLUE);
    const int resetPuzzleWidth = MeasureText("Reset Puzzle", 20);
    DrawText("Reset Puzzle", 330 + (150 - resetPuzzleWidth) * 0.5, 15, 20, clickingResetPuzzle ? GRAY : WHITE);
    DrawLineEx((Vector2) { 0, 50 }, (Vector2) { screenWidth, 50 }, 3, DARKBLUE);
    for (int i = 0; i < TILE_COUNT; ++i)
    {
        const Vector2 source = (Vector2) { tiles[i] % tileColumns * tileSize, photoHeight - tileSize - tiles[i] / tileColumns * tileSize };
        const Vector2 destination = (Vector2) { i % tileColumns * tileSize, i / tileColumns * tileSize + 50 };
        DrawTextureRec(renderTexture.texture, (Rectangle) { source.x, source.y, tileSize, -tileSize }, destination, WHITE);
    }
    if (selectedTileIndex != -1)
    {
        DrawRectangle(selectedTileIndex % tileColumns * tileSize, selectedTileIndex / tileColumns * tileSize + 50, tileSize, tileSize, (Color) { 255, 255, 0, 128 });
    }
    EndDrawing();
}

static void Terminate()
{
    CloseWindow();
    UnloadSound(buttonSound);
    UnloadSound(selectTileSound);
    UnloadSound(swapTileSound);
    UnloadSound(completeSound);
    for (int i = 0; i < PHOTO_COUNT; ++i)
    {
        UnloadTexture(photos[i]);
    }
    UnloadRenderTexture(renderTexture);
    CloseAudioDevice();
}

static void ResetTiles()
{
    for (int i = 0; i < TILE_COUNT; ++i)
    {
        tiles[i] = i;
    }
}

static void ScrambleTiles()
{
    ResetTiles();
    for (int i = 0; i < TILE_COUNT; ++i)
    {
        const int swapIndex = GetRandomValue(0, TILE_COUNT - 1);
        const int tileValue = tiles[i];
        tiles[i] = tiles[swapIndex];
        tiles[swapIndex] = tileValue;
    }
}

static bool IsComplete()
{
    for (int i = 0; i < TILE_COUNT; ++i)
    {
        if (tiles[i] != i)
        {
            return false;
        }
    }
    return true;
}