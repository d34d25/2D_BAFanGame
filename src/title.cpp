#include "title.h"

#include <cmath>

void Title::DrawTitle()
{
    ClearBackground(BLACK);

    BeginTextureMode(titleCanvas);

    DrawTexture(
        titleScreen, 0,0, WHITE
    );

    Vector2 startTextPos = SetUIElementPosition(5, 8);

    if(lowFrequencyCounter >= MAX_COUNT * 0.5f) DrawText("PRESS START!", startTextPos.x, startTextPos.y, 1, WHITE);

    if(debugDrawing)
    {
        for(int i = 0; i <= CANVAS_WIDTH; i+= TILE_SIZE)
        {
            DrawLineEx(
                {(float)i, 0.0f},
                {(float)i, (float)NATIVE_HEIGHT},
                1.0f,
                MAGENTA
            );
        }

        for(int i = 0; i <= NATIVE_HEIGHT; i+= TILE_SIZE)
        {
            DrawLineEx(
                {0.0f, (float)i},
                {(float)CANVAS_WIDTH, (float)i},
                1.0f,
                MAGENTA
            );
        }
    }

    EndTextureMode();

    float scale = fminf((float)GetScreenWidth() / NATIVE_WIDTH, 
    (float)GetScreenHeight() / NATIVE_HEIGHT);

    float offsetX = ((float)GetScreenWidth() - ((float)NATIVE_WIDTH * scale)) * 0.5f;
    float offsetY = ((float)GetScreenHeight() - ((float)NATIVE_HEIGHT * scale)) * 0.5f;
    
    Rectangle sourceTitleRec = {
        0,0,
        (float)titleCanvas.texture.width,
        (float)-titleCanvas.texture.height
    };

    Rectangle destTitleRec = {
        offsetX,
        offsetY,
        titleCanvas.texture.width * scale,
        titleCanvas.texture.height * scale
    };

    DrawTexturePro(
        titleCanvas.texture,
        sourceTitleRec,
        destTitleRec,
        {0,0},
        0.0f,
        WHITE
    );
}

void Title::UpdateTitle()
{
    lowFrequencyCounter++;

    if(lowFrequencyCounter >= MAX_COUNT) lowFrequencyCounter = 0;

    if(optionPressed)
    {
        contToGameplay = true;

        optionPressed = false;
    }
}

void Title::UpdateTitleInput()
{
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_Z))
        optionPressed = true;

    if(IsKeyPressed(KEY_UP)) pressedUp = true;

    if(IsKeyPressed(KEY_DOWN)) pressedDown = true;

    if(IsKeyPressed(KEY_D)) debugDrawing = true;
    else if(IsKeyPressed(KEY_F)) debugDrawing = false;
}
