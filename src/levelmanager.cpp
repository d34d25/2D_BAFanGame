#include "levelmanager.h"

Title title = {};

RenderTexture2D gameplayCanvas = {};

RenderTexture2D uiCanvas = {};

std::unique_ptr<Level> currLevel = nullptr;

std::unique_ptr<LevelEditor> editor = nullptr;

GameScreen currentScreen = GameScreen::TITLE;

int currentLevelIndex = 0;

bool levelReady = false;

void UpdateScreen(float dt, int iterations)
{
    switch (currentScreen)
    {
    case GameScreen::GAMEPLAY:
    {
        if(!currLevel->LevelStarted()) currLevel->TickLevelStartTimer();

        if(!currLevel->paused) currLevel->UpdateLevel();
        else currLevel->Pause();

        if(currLevel->retToTitle)
        {
            currentScreen = GameScreen::TITLE;

            title.ResetTitle();
        }
    }
    break;
    
    case GameScreen::EDITOR: editor->Update(); break;
    
    case GameScreen::TITLE:
    {
        title.UpdateTitle();

        if(title.contToGameplay)
        {
            currentScreen = GameScreen::GAMEPLAY;

            /*ResetLevel(
                "levels/testLevel",
                "levels/testRooms",
                dt,
                iterations
            );*/
        }

    }
    break;

    default: break;
    }
}

void UpdateScreenInput()
{
    switch (currentScreen)
    {
    case GameScreen::GAMEPLAY:
    {
        if(currLevel->LevelStarted())
        {
            currLevel->UpdatePlayerInput();

            currLevel->EvaluateLevelPause();
        }
    } 
    break;

    case GameScreen::TITLE: title.UpdateTitleInput(); break;

    default: break;
    }
}

void DrawScreen()
{
    switch (currentScreen)
    {
    case GameScreen::GAMEPLAY:
    {
        if(levelReady) currLevel->DrawLevel();
        else ClearBackground(BLACK);
        
    }
    break;
        
    case GameScreen::EDITOR: editor->Draw(); break;

    case GameScreen::TITLE: title.DrawTitle(); break;

    default: break;
    }
}
