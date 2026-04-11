#include "leveldata.h"

//static
SpriteRenderData solidTilesRenderData = {};

SpriteRenderData spikesRenderData = {};

//animated
SpriteRenderData treadmillRenderData_Right = {};

SpriteRenderData treadmillRenderData_Left = {};

void LoadRenderData(const char* path, SpriteRenderData* renderData, int maxFrames, Vector2 frameSize, int spacing = 1, int startFrame = 0, int endFrame = 0)
{
    renderData->animationFrames.clear();

    renderData->sourceTexture = LoadTexture(path);

    renderData->maxFrames = maxFrames;

    renderData->frameSize = frameSize;

    renderData->animationFrames = CropImage(renderData->sourceTexture, renderData->maxFrames, renderData->frameSize);

    renderData->startFrame = startFrame;

    if(endFrame < 1 && !renderData->animationFrames.empty()) endFrame = (int)renderData->animationFrames.size();

    renderData->endFrame = endFrame;

    renderData->spacing = spacing;

    SetTextureFilter(renderData->sourceTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(renderData->sourceTexture, TEXTURE_WRAP_CLAMP);
}

void LoadAssets()
{
    Vector2 tileSize = {16,16};

    LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet.png", &solidTilesRenderData, 9, tileSize);

    //spikes

    LoadRenderData("assets/tiles/spike-sprite-sheet.png", &spikesRenderData, 5, tileSize);

    //treadmills

    int treadmillMaxFrames = 6 * 2;

    LoadRenderData("assets/tiles/treadmill-spritesheet.png", &treadmillRenderData_Right, treadmillMaxFrames, tileSize, 2, 0, (int)(treadmillMaxFrames / 2));

    LoadRenderData("assets/tiles/treadmill-spritesheet.png", &treadmillRenderData_Left, treadmillMaxFrames, tileSize, 2, (int)(treadmillMaxFrames / 2), treadmillMaxFrames);
}

void UnloadAssets()
{
    solidTilesRenderData.animationFrames.clear();

    treadmillRenderData_Right.animationFrames.clear();

    spikesRenderData.animationFrames.clear();
}
