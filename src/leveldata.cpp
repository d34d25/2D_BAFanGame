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

//enemies

std::vector<SpriteRenderData> dummyRenderData = {};

std::vector<SpriteRenderData> yuukaRenderData = {};

//enemies' weapons

std::vector<SpriteRenderData> yuukaWeaponRenderData = {};

//player

std::vector<SpriteRenderData> momoiRenderData = {};
std::vector<SpriteRenderData> midoriRenderData = {};

std::vector<SpriteRenderData> yuzuRenderData = {};
std::vector<SpriteRenderData> arisRenderData = {};

//player's weapons

std::vector<SpriteRenderData> momoiWeaponRenderData = {};

std::vector<SpriteRenderData> midoriWeaponRenderData = {};

std::vector<SpriteRenderData> yuzuWeaponRenderData = {};
std::vector<SpriteRenderData> arisWeaponRenderData = {};

/*
    spacing is how many frames an animation loop has, this value is fixed for each SpriteRenderData
    if the spacing is 0 or less the whole animation vector will be used, if it is 1 the SpriteRenderData won't have animations,
    if it is greater than 1, each animation will have <spacing> frames per animation

    spacing is only used for time based animations (passive animations such as the objects of the environment)

    atlasStartFrame and atlasEndFrame indicates what portion of the spritesheet is used
    these values are fixed for each SpriteRenderData

    CropImage splits the given texture in a uniform grid with a 1px gap

    each SpriteRenderData will have a fixed spacing and frameSize value, but different SpriteRenderData can use
    the same source texture despite having differente sizes, start, end and spacing.
*/

SpriteRenderData LoadRenderData(const char* path, Vector2 frameSize, Vector2 offset, int spacing, int atlasStartFrame, int atlasEndFrame, float animationSpeed)
{
    SpriteRenderData renderData = {};

    renderData.offset = offset;

    renderData.ogOffset = renderData.offset;

    renderData.spacing = spacing;

    renderData.animationSpeed = animationSpeed;

    renderData.animationFrames.clear();

    renderData.frameSize = frameSize;

    std::string key = path;

    if(textureCache.find(key) == textureCache.end())
    {
        textureCache[key] = LoadTexture(path);

        SetTextureFilter(textureCache[key], TEXTURE_FILTER_POINT);

        SetTextureWrap(textureCache[key], TEXTURE_WRAP_CLAMP);
    }

    renderData.sourceTexture = &textureCache[key];

    std::vector<Rectangle> allFrames = CropImage(*renderData.sourceTexture, renderData.frameSize, 1);

    if(atlasEndFrame <= 0 || atlasEndFrame > (int)allFrames.size()) atlasEndFrame = (int)allFrames.size();

    if(atlasStartFrame < 0 || atlasStartFrame > atlasEndFrame) atlasStartFrame = 0;

    if(!allFrames.empty() && atlasStartFrame < atlasEndFrame)
    {
        renderData.animationFrames.assign(
            allFrames.begin() + atlasStartFrame,
            allFrames.begin() + atlasEndFrame
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

    treadmillRenderData_Right.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", tileSize, {0,0} ,2, 0, 6));

    treadmillRenderData_Left.push_back(LoadRenderData("assets/tiles/treadmill-spritesheet.png", tileSize, {0,0}, 2, 6, 12));

    //decoration

    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-1.png", {14,16}, {0,0},2, 0, 0, 2.0f));
    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-2.png", {14,16}));
    decoRenderData.push_back(LoadRenderData("assets/tiles/deco/deco-spritesheet-3.png", {35,29}));

    //wind tiles

    windRenderData.push_back(LoadRenderData("assets/tiles/wind-up.png", {16,16},{0,0},3));

    //water

    waterRenderData.push_back(LoadRenderData("assets/tiles/water.png", {16,16}, {0,0}, 3));

    //platforms

    movingPlatformRenderData_Vertical.push_back(LoadRenderData("assets/platforms/vertical-moving-platform-spritesheet.png", {48,5}, {0,0}, 2));

    //enemies

    dummyRenderData.push_back(LoadRenderData("assets/enemies/chibi-dummy.png", {8,12}));
    
    yuukaRenderData.push_back(LoadRenderData("assets/enemies/yuuka-sprtiesheet.png", {20,22}, {-4,-4}, 1, 0, 0, 10.0f));

    //enemies' weapons

    yuukaWeaponRenderData.push_back(LoadRenderData("assets/enemies/yuuka-weapon-holo.png", {10,4}, {39,7}));

    //player

    momoiRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-spritesheet-b.png",
        {14.0f, 24.0f},
        {0.0f, -7.0f},
        1,0,0, 
        10.0f
    ));

    momoiRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-chaquena-spritesheet.png",
        {18.0f, 23.0f},
        {0.0f, -7.0f},
        1,0,0, 
        10.0f
    ));

    midoriRenderData.push_back(LoadRenderData(
        "assets/characters/midori-spritesheet.png",
        {14.0f, 24.0f},
        {0.0f, -7.0f},
        1, 0, 0,
        10.0f
    ));

    yuzuRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-spritesheet.png",
        {21.0f, 22.0f},
        {-4.0f, -4.0f},
        1, 0, 0,
        10.0f
    ));

    yuzuRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-battle-spritesheet.png",
        {18.0f, 24.0f},
        {-6.0f, -8.0f},
        1, 0, 0,
        10.0f
    ));

    arisRenderData.push_back(LoadRenderData(
        "assets/characters/aris-spritesheet.png",
        {17.0f, 21.0f},
        {0.0f, -3.0f},
        1, 0, 0,
        10.0f
    ));

    //player's weapons

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-weapon-holo.png",
        {10.0f, 5.0f},
        {39.0f, 7.0f}
    ));

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-chaquena-weapon-holo.png",
        {5.0f, 7.0f},
        {39.0f, 7.0f}
    ));

    midoriWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/midori-weapon-holo.png",
        {11.0f, 5.0f},
        {42.0f, 7.0f}
    ));

    yuzuWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-weapon-holo.png",
        {10.0f, 5.0f},
        {42.0f, 7.0f}
    ));

    arisWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/aris-weapon-holo.png",
        {15.0f, 4.0f},
        {51.0f, 7.0f}
    ));
}

void UnloadAssets()
{
    auto CleanUp = [](std::vector<SpriteRenderData>& renderData)
    {
        for(auto& data : renderData)
        {
            data.animationFrames.clear();

            data.animationFrames.shrink_to_fit();
        }

        renderData.clear();

        renderData.shrink_to_fit();
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

    //enemies

    CleanUp(dummyRenderData);

    CleanUp(yuukaRenderData);

    //enemies' weapons

    CleanUp(yuukaWeaponRenderData);

    //player

    CleanUp(momoiRenderData);
    CleanUp(midoriRenderData);

    CleanUp(yuzuRenderData);
    CleanUp(arisRenderData);

    //player's weapons

    CleanUp(momoiWeaponRenderData);
    CleanUp(midoriWeaponRenderData);

    CleanUp(yuzuWeaponRenderData);
    CleanUp(arisWeaponRenderData);
}
