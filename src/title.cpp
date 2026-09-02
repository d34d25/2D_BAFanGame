#include "title.h"

#include <cmath>

void Title::DrawStartScreen()
{
    Vector2 startTextPos = SetUIElementPosition(5, 8);

    Vector2 zTextPos = SetUIElementPosition(7, 10);

    if(lowFrequencyCounter >= MAX_COUNT * 0.5f)
    {
        DrawText("PRESS START!", startTextPos.x, startTextPos.y, 1, WHITE);

        DrawText("(Z/Enter)", zTextPos.x, zTextPos.y, 1, WHITE);
    }
}

void Title::DrawCharSelection()
{
    Vector2 charSelTextPos = SetUIElementPosition(4, 1);

    if(lowFrequencyCounter >= MAX_COUNT * 0.5f) DrawText("SELECT CHARACTER", charSelTextPos.x, charSelTextPos.y, 1, WHITE);

    std::vector<Rectangle> frameRecs = {};

    for(int i = 0; i < framePos.size(); i++)
    {
        Rectangle recFrame = {
            framePos[i].x,
            framePos[i].y,
            CHAR_SEL_FRAME_SIZE,
            CHAR_SEL_FRAME_SIZE
        };

        DrawRectangleLinesEx(recFrame, 1, WHITE);

        const char* text = "";

        switch (i)
        {
        case 0: text = "Yuzu"; break;

        case 1: text = "Momoi"; break;

        case 2: text = "Midori"; break;

        case 3: text = "Aris"; break;
        
        default: break;
        }

        DrawText(text, recFrame.x + recFrame.width + xPos * 2,
        recFrame.y + recFrame.height * 0.5f, 1, WHITE);

        frameRecs.push_back(recFrame);
    }

    BeginShaderMode(paletteShader);

    ChangePalette(0, &spritePalettes);

    DrawSprite(
        &yuzuRenderData[0],
        framePos[0].x + (CHAR_SEL_FRAME_SIZE * 0.5f),
        framePos[0].y + (CHAR_SEL_FRAME_SIZE * 0.5f),
        0
    );

    ChangePalette(1, &spritePalettes);

    DrawSprite(
        &momoiRenderData[0],
        framePos[1].x + (CHAR_SEL_FRAME_SIZE * 0.5f),
        framePos[1].y + (CHAR_SEL_FRAME_SIZE * 0.5f),
        0
    );

    ChangePalette(2, &spritePalettes);

    DrawSprite(
        &midoriRenderData[0],
        framePos[2].x + (CHAR_SEL_FRAME_SIZE * 0.5f),
        framePos[2].y + (CHAR_SEL_FRAME_SIZE * 0.5f),
        0
    );

    ChangePalette(3, &spritePalettes);

    DrawSprite(
        &arisRenderData[0],
        framePos[3].x + (CHAR_SEL_FRAME_SIZE * 0.5f),
        framePos[3].y + (CHAR_SEL_FRAME_SIZE * 0.5f),
        0
    );

    EndShaderMode();

    DrawText("<", 10 * TILE_SIZE, selectorPosY, 1, WHITE);
}

void Title::InitTitle()
{
    Vector2 yuzuFramePos = SetUIElementPosition(xPos, yPos);

    Vector2 momoiFramePos = SetUIElementPosition(xPos, yPos * 2);

    Vector2 midoriFramePos = SetUIElementPosition(xPos, yPos * 3);

    Vector2 arisFramePos = SetUIElementPosition(xPos, yPos * 4);

    framePos = {yuzuFramePos, momoiFramePos, midoriFramePos, arisFramePos};

    selectorPosY = framePos[0].y + CHAR_SEL_FRAME_SIZE * 0.5f;
}

void Title::DrawTitle()
{
    ClearBackground(BLACK);

    BeginTextureMode(titleCanvas);

    if(inCharSelection)
    {
        ClearBackground(BLACK);

        DrawCharSelection();
    } 
    else
    {
        DrawTexture(titleScreen, 0,0, WHITE);

        DrawStartScreen();
    }

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

    if(optionPressed && !inCharSelection)
    {
        inCharSelection = true;

        optionPressed = false;
    }
    else if(optionPressed)
    {
        contToGameplay = true;

        optionPressed = false;
    }

    if(inCharSelection)
    {
        int step = 4 * TILE_SIZE;

        float min = framePos[0].y + CHAR_SEL_FRAME_SIZE * 0.5f;
        float max = framePos[3].y + CHAR_SEL_FRAME_SIZE * 0.5f;

        float middleA = framePos[1].y + CHAR_SEL_FRAME_SIZE * 0.5f;
        float middleB = framePos[2].y + CHAR_SEL_FRAME_SIZE * 0.5f;

        if(pressedDown)
        {
            selectorPosY += step;

            if(selectorPosY >= max) selectorPosY = max;

            pressedDown = false;
            pressedUp = false;
        }
        else if(pressedUp)
        {
            selectorPosY -= step;

            if(selectorPosY <= min) selectorPosY = min;

            pressedDown = false;
            pressedUp = false;
        }

        if(selectorPosY == min) selectedCharacter = Character::YUZU;
        else if(selectorPosY == middleA) selectedCharacter = Character::MOMOI;
        else if(selectorPosY == middleB) selectedCharacter = Character::MIDORI;
        else if(selectorPosY == max) selectedCharacter = Character::ARIS;

        /*const char* characterSlctd = "";

        switch (selectedCharacter)
        {
        case Character::YUZU: characterSlctd = "YUZU"; break;
        
        case Character::MOMOI: characterSlctd = "MOMOI"; break;

        case Character::MIDORI: characterSlctd = "MIDORI"; break;

        case Character::ARIS: characterSlctd = "ARIS"; break;
        default: break;
        }

        std::cout<<characterSlctd<<"\n";*/
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
