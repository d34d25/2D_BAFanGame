#include "leveldata.h"

//static
SpriteRenderData solidTilesRenderData = {};

SpriteRenderData spikesRenderData = {};

//animated
SpriteRenderData treadmillRenderData = {};

void LoadRenderData(const char* path, SpriteRenderData* renderData, int maxFrames, Vector2 size)
{
    renderData->animationFrames.clear();

    renderData->sourceTexture = LoadTexture(path);

    renderData->animationFrames = CropImage(renderData->sourceTexture, maxFrames, size);

    SetTextureFilter(renderData->sourceTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(renderData->sourceTexture, TEXTURE_WRAP_CLAMP);
}

void LoadAssets()
{
    Vector2 tileSize = {(float)gridSize,(float)gridSize};

    LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet.png", &solidTilesRenderData, 9, tileSize);

    //spikes

    LoadRenderData("assets/tiles/spike-sprite-sheet.png", &spikesRenderData, 5, tileSize);

    //treadmills

    LoadRenderData("assets/tiles/treadmill-right-spritesheet.png", &treadmillRenderData, 8, tileSize);
}

void UnloadAssets()
{
    solidTilesRenderData.animationFrames.clear();

    treadmillRenderData.animationFrames.clear();

    spikesRenderData.animationFrames.clear();
}
