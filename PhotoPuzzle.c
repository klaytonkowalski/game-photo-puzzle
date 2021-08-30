#include "raylib.h"

#include <stdio.h>

#define PHOTO_COUNT 9
#define TILE_COUNT 144

static const int photoWidth = 960;
static const int photoHeight = 540;
static const int screenWidth = 960;
static const int screenHeight = 590;
static const int targetFPS = 90;
static const int tileSize = 60;
static const int tileColumns = 16;

static Sound buttonSound;
static Sound selectTileSound;
static Sound swapTileSound;
static Sound completeSound;
static Texture photos[PHOTO_COUNT];
static RenderTexture renderTexture;

static int photoIndex;
static int selectedTileIndex;
static int tiles[TILE_COUNT];
static bool clickingNewPuzzle;
static bool clickingSolvePuzzle;
static bool clickingResetPuzzle;

static void Initialize();
static void Update();
static void Draw();
static void Terminate();
static void ResetTiles();
static void ScrambleTiles();
static bool IsComplete();

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
    BeginTextureMode(renderTexture);
    DrawTexture(photos[photoIndex], 0, 0, WHITE);
    EndTextureMode();
    BeginDrawing();
    ClearBackground(WHITE);
    DrawRectangle(0, 0, screenWidth, 50, SKYBLUE);
    DrawRectangle(10, 10, 150, 30, clickingNewPuzzle ? DARKBLUE : BLUE);
    DrawText("New Puzzle", 15, 15, 20, clickingNewPuzzle ? GRAY : WHITE);
    DrawRectangle(170, 10, 150, 30, clickingSolvePuzzle ? DARKBLUE : BLUE);
    DrawText("Solve Puzzle", 175, 15, 20, clickingSolvePuzzle ? GRAY : WHITE);
    DrawRectangle(330, 10, 150, 30, clickingResetPuzzle ? DARKBLUE : BLUE);
    DrawText("Reset Puzzle", 335, 15, 20, clickingResetPuzzle ? GRAY : WHITE);
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