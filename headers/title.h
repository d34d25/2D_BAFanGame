#pragma once

#include "raylib.h"

#include "definitions.h"

#include "drawing.h"

const int MAX_COUNT = 16;

struct Title
{
    RenderTexture2D titleCanvas = {};

    Texture2D titleScreen = {};

    int lowFrequencyCounter = 0;

    bool optionPressed = false;

    bool pressedUp = false;

    bool pressedDown = false;

    bool contToGameplay = false;

    bool debugDrawing = false;

    void DrawTitle();

    void UpdateTitle();

    void UpdateTitleInput();

    inline void ResetTitle()
    {
        optionPressed = false;

        pressedDown = false;

        pressedUp = false;

        contToGameplay = false;

        debugDrawing = false;
    }
};
