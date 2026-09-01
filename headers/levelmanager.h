#pragma once

#include "level.h"

#include "levelEditor.h"

#include "title.h"

enum struct GameScreen
{
    TITLE,
    GAMEPLAY,
    EDITOR,
    CREDITS
};

extern Title title;

extern RenderTexture2D gameplayCanvas;

extern RenderTexture2D uiCanvas;

extern std::unique_ptr<Level> currLevel;

extern std::unique_ptr<LevelEditor> editor;

extern GameScreen currentScreen;

extern int currentLevelIndex;

extern bool levelReady;

inline void InitManagerExtern()
{
    LoadAssets();

    title.titleScreen = LoadTexture("assets/ui/titleScreen.png");

    title.titleCanvas = LoadRenderTexture(NATIVE_WIDTH, NATIVE_HEIGHT);

    gameplayCanvas = LoadRenderTexture(CANVAS_WIDTH, GAMEPLAY_CANVAS_HEIGHT);

    uiCanvas = LoadRenderTexture(CANVAS_WIDTH, UI_CANVAS_HEIGHT);

    SetTextureFilter(title.titleCanvas.texture, TEXTURE_FILTER_POINT);

    SetTextureFilter(gameplayCanvas.texture, TEXTURE_FILTER_POINT);

    SetTextureFilter(uiCanvas.texture, TEXTURE_FILTER_POINT);

    currentScreen = GameScreen::TITLE;

    currentLevelIndex = 0;

    currLevel = std::make_unique<Level>();

    editor = std::make_unique<LevelEditor>(NATIVE_WIDTH, NATIVE_HEIGHT, "levels/testLevel", "levels/testRooms");
}

inline void ResetLevel(
    const char* levelPath,
    const char* roomPath,
    float dt,
    int iterations
)
{
    std::cout<<"level not ready\n";

    levelReady = false;

    currLevel.reset();

    currLevel = std::make_unique<Level>();

    currLevel->InitLevel(levelPath, roomPath, dt, iterations);
    
    currLevel->gameplayCanvas = gameplayCanvas;
    currLevel->uiCanvas = uiCanvas;

    currLevel->ResetLevel();

    levelReady = true;

    std::cout<<"level ready\n";
}

inline void DestroyManager()
{
    currLevel.reset();
    
    editor.reset();

    UnloadAssets();

    UnloadRenderTexture(gameplayCanvas);

    UnloadRenderTexture(uiCanvas);
}

void UpdateScreenInput();

void UpdateScreen(float dt, int iterations);

void DrawScreen();