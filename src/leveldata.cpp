#include "leveldata.h"

std::vector<SpriteRenderData> solidTilesRenderData = {};

std::vector<SpriteRenderData> spikesRenderData = {};

std::vector<SpriteRenderData> treadmillRenderData_Right = {};

std::vector<SpriteRenderData> treadmillRenderData_Left = {};

std::vector<SpriteRenderData> decoRenderData = {};

/*
    spacing means the length of each animation, if it is 1 frame it means it's static
    start and end frame are only used in case of having multiple variants in one sprite sheet

    sprite sheets are treated as an uniform grid so a single sprite sheet regadless of its content must have
    a constant spacing and frame size
*/
SpriteRenderData LoadRenderData(const char* path, int maxFrames, Vector2 frameSize, int spacing = 1, int startFrame = 0, int endFrame = 0, float animationSpeed = 5.0f)
{
    SpriteRenderData renderData = {};

    renderData.animationFrames.clear();

    renderData.sourceTexture = LoadTexture(path);

    renderData.maxFrames = maxFrames;

    renderData.frameSize = frameSize;

    renderData.animationFrames = CropImage(renderData.sourceTexture, renderData.maxFrames, renderData.frameSize);

    renderData.startFrame = startFrame;

    if(endFrame < 1 && !renderData.animationFrames.empty()) endFrame = (int)renderData.animationFrames.size();

    renderData.endFrame = endFrame;

    renderData.spacing = spacing;

    renderData.animationSpeed = animationSpeed;

    SetTextureFilter(renderData.sourceTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(renderData.sourceTexture, TEXTURE_WRAP_CLAMP);

    return renderData;
}

void LoadAssets()
{
    Vector2 tileSize = {16,16};

    solidTilesRenderData.push_back(LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet.png", 9, tileSize));

    solidTilesRenderData.push_back(LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet-b.png", 9, tileSize));

    //spikes

    spikesRenderData.push_back(LoadRenderData("assets/tiles/spike-sprite-sheet.png", 5, tileSize));

    //treadmills

    int treadmillMaxFrames = 6 * 2;

    treadmillRenderData_Right.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", treadmillMaxFrames, tileSize, 2, 0, (int)(treadmillMaxFrames / 2)));

    treadmillRenderData_Left.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", treadmillMaxFrames, tileSize, 2, (int)(treadmillMaxFrames / 2), treadmillMaxFrames));

    //decoration

    decoRenderData.push_back(LoadRenderData("assets/tiles/deco-spritesheet-1.png", 2, {14,16}, 2, 0, 0, 2.0f));
    decoRenderData.push_back(LoadRenderData("assets/tiles/deco-spritesheet-2.png", 2, {14,15}));
}

void UnloadAssets()
{
    auto CleanUp = [](std::vector<SpriteRenderData>& renderData)
    {
        for(auto& data : renderData)
        {
            data.animationFrames.clear();
        }

        renderData.clear();
    };

    CleanUp(solidTilesRenderData);

    CleanUp(spikesRenderData);

    CleanUp(treadmillRenderData_Left);

    CleanUp(treadmillRenderData_Right);

    CleanUp(decoRenderData);
}
