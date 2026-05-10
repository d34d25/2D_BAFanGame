#include "leveldata.h"

#include <map>
#include <string>

static std::map<std::string, Texture2D> textureCache = {};

//tiles
std::vector<SpriteRenderData> solidTilesRenderData = {};

std::vector<SpriteRenderData> treadmillRenderData_Right = {};

std::vector<SpriteRenderData> treadmillRenderData_Left = {};

std::vector<SpriteRenderData> decoRenderData = {};

//spikes

std::vector<SpriteRenderData> spikesRenderData = {};

std::vector<SpriteRenderData> spikesDobuleRenderData = {};

std::vector<SpriteRenderData> spikesSmallRenderData = {};

//wind tiles

std::vector<SpriteRenderData> windRenderData = {};

//water

std::vector<SpriteRenderData> waterRenderData = {};

//platforms

std::vector<SpriteRenderData> movingPlatformRenderData_Vertical = {};

/*
    spacing is how many frames an animation loop has, this value is fixed for each SpriteRenderData
    if the spacing is 0 or less the whole animation vector will be used, if it is 1 the SpriteRenderData won't have animations,
    if it is greater than 1, each animation will have <spacing> frames per animation

    startFrame and endFrames alongside frameSize indicates what portion of the spritesheet is used
    these values are fixed for each SpriteRenderData

    CropImage splits the given texture in a uniform grid with a 1px gap

    each SpriteRenderData will have a fixed spacing and frameSize value, but different SpriteRenderData can use
    the same source texture despite having differente sizes, start, end and spacing.
*/
SpriteRenderData LoadRenderData(const char* path, Vector2 frameSize, int spacing = 1, int startFrame = 0, int endFrame = 0, float animationSpeed = 5.0f)
{
    SpriteRenderData renderData = {};

    renderData.spacing = spacing;

    renderData.animationSpeed = animationSpeed;

    renderData.animationFrames.clear();

    std::string key = path;

    if(textureCache.find(key) == textureCache.end())
    {
        textureCache[key] = LoadTexture(path);

        SetTextureFilter(textureCache[key], TEXTURE_FILTER_POINT);

        SetTextureWrap(textureCache[key], TEXTURE_WRAP_CLAMP);
    }

    renderData.sourceTexture = &textureCache[key];

    std::vector<Rectangle> allFrames = CropImage(*renderData.sourceTexture, frameSize, 1);

    if(endFrame <= 0 || endFrame > (int)allFrames.size()) endFrame = (int)allFrames.size();

    if(startFrame < 0 || startFrame > endFrame) startFrame = 0;

    if(!allFrames.empty() && startFrame < endFrame)
    {
        renderData.animationFrames.assign(
            allFrames.begin() + startFrame,
            allFrames.begin() + endFrame
        );
    }

    renderData.maxFrames = (int)renderData.animationFrames.size();

    return renderData;
}

void LoadAssets()
{
    Vector2 tileSize = {16,16};

    //tiles

    solidTilesRenderData.push_back(LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet.png", tileSize));

    solidTilesRenderData.push_back(LoadRenderData("assets/tiles/solid/solid-tiles-spritesheet-b.png", tileSize));

    //spikes

    spikesRenderData.push_back(LoadRenderData("assets/tiles/spike-sprite-sheet.png", tileSize));

    spikesDobuleRenderData.push_back(LoadRenderData("assets/tiles/spike-double-sprite-sheet.png", tileSize));

    spikesSmallRenderData.push_back(LoadRenderData("assets/tiles/spike-small-sprite-sheet.png", tileSize));

    //treadmills

    treadmillRenderData_Right.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", tileSize, 2, 0, 6));

    treadmillRenderData_Left.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", tileSize, 2, 6, 12));

    //decoration

    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-1.png", {14,16}, 2, 0, 0, 2.0f));
    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-2.png", {14,16}));
    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-3.png", {35,29}));

    //wind tiles

    windRenderData.push_back(LoadRenderData("assets/tiles/wind-up.png", {16,16}, 3));

    //water

    waterRenderData.push_back(LoadRenderData("assets/tiles/water.png", {16,16}, 3));

    //platforms

    movingPlatformRenderData_Vertical.push_back(LoadRenderData("assets/platforms/vertical-moving-platform-spritesheet.png", {48,5}, 2));
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

    //tiles
    CleanUp(solidTilesRenderData);

    CleanUp(treadmillRenderData_Left);

    CleanUp(treadmillRenderData_Right);

    CleanUp(decoRenderData);

    //spikes

    CleanUp(spikesRenderData);

    CleanUp(spikesDobuleRenderData);

    CleanUp(spikesSmallRenderData);

    //platforms

    CleanUp(movingPlatformRenderData_Vertical);

    CleanUp(windRenderData);

    CleanUp(waterRenderData);
}
