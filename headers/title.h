#pragma once

#include "raylib.h"

#include "definitions.h"

#include "drawing.h"

#include "leveldata.h"

#include <vector>

#include <iostream>

const int MAX_COUNT = 16;

const int CHAR_SEL_FRAME_SIZE = 24;

struct Title
{
    std::vector<Vector2> framePos = {};

    RenderTexture2D titleCanvas = {};

    Texture2D titleScreen = {};

    Vector2 charSelectPos = {};

    Character selectedCharacter = Character::YUZU;

    int selectorPosY = 0;

    int lowFrequencyCounter = 0;

    int xPos = 2;

    int yPos = 4;

    bool optionPressed = false;

    bool pressedUp = false;

    bool pressedDown = false;

    bool contToGameplay = false;

    bool debugDrawing = false;

    bool inCharSelection = false;

    void InitTitle();

    void DrawTitle();

    void UpdateTitle();

    void UpdateTitleInput();

    void DrawStartScreen();

    void DrawCharSelection();

    inline void ResetTitle()
    {
        optionPressed = false;

        pressedDown = false;

        pressedUp = false;

        contToGameplay = false;

        debugDrawing = false;

        inCharSelection = false;
    }
};
