#include "leveldata.h"

#include <map>

#include <string>

static std::map<std::string, Texture2D> textureCache = {};

//tiles
std::vector<SpriteRenderData> solidTilesRenderData = {};

std::vector<SpriteRenderData> treadmillRenderData_Right = {};

std::vector<SpriteRenderData> treadmillRenderData_Left = {};

std::vector<SpriteRenderData> decoRenderData = {};

std::vector<SpriteRenderData> ladderRenderData = {};

//spikes

std::vector<SpriteRenderData> spikeRenderData = {};

std::vector<SpriteRenderData> spikeDobuleRenderData = {};

std::vector<SpriteRenderData> spikeSmallRenderData = {};

std::vector<SpriteRenderData> spikeBallRenderData = {};

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

//UI

Texture2D uiBackground = {};

std::vector<SpriteRenderData> portraits = {};

std::vector<SpriteRenderData> uiElements = {};

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

/*
end - start = quantity of frames
start atlas frame inclusive, 0 based offset
end atlas frame is exclusive, is the total count boundary
for consecutive blocks the end index of the previous
is the start of the next

start atlas frame is the left side of the sprite
end atlas frame is the right side
*/

void LoadAssets()
{
    Vector2 tileSize = {12,12};

    //tiles

    const int TOTAL_COLUMNS_FILES = 10;

    for(int i = 1; i <= TOTAL_COLUMNS_FILES; i++)
    {
        std::string filepath = "assets/tiles/solid/columns-" + std::to_string(i) + ".png";

        solidTilesRenderData.push_back(LoadRenderData(
            filepath.c_str(), tileSize
        ));
    }

    for(int i = 1; i <= TOTAL_COLUMNS_FILES; i++)
    {
        std::string filepath = "assets/tiles/solid/yellow-columns-" + std::to_string(i) + ".png";

        solidTilesRenderData.push_back(LoadRenderData(
            filepath.c_str(), tileSize
        ));
    }

    const int TOTAL_BLOCKS_FILES = 8;

    for(int i = 1; i <= TOTAL_BLOCKS_FILES; i++)
    {
        std::string filepath = "assets/tiles/solid/blocks-" + std::to_string(i) + ".png";

        solidTilesRenderData.push_back(LoadRenderData(
            filepath.c_str(), tileSize
        ));
    }

    const int TOTAL_YELLOW_BLOCKS_FILES = 10;

    for(int i = 1; i <= TOTAL_YELLOW_BLOCKS_FILES; i++)
    {
        std::string filepath = "assets/tiles/solid/yellow-blocks-" + std::to_string(i) + ".png";

        solidTilesRenderData.push_back(LoadRenderData(
            filepath.c_str(), tileSize
        ));
    }

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/block-panels.png", tileSize
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/yellow-block-panels.png", tileSize
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/fixed-color-tiles.png", tileSize
    ));

    ladderRenderData.push_back(LoadRenderData("assets/tiles/ladder.png", tileSize));

    //panels

    float panelAnimationSpeed = 4.25f;

    Vector2 dTileSize = Vector2Scale(tileSize, 2);

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-a.png", dTileSize, dTileSize
    ));
    
    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-b.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-c.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-d.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-e.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-f.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/filler-blocks-g.png", dTileSize, dTileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/shadows.png", tileSize
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/gdd-cat-gbc.png", {35,29}
    ));

    int panelSpacing = 12;

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        0, 12,
        panelAnimationSpeed
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        12, 24,
        panelAnimationSpeed
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        24, 36,
        panelAnimationSpeed
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        36, 48,
        panelAnimationSpeed
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        48, 60,
        panelAnimationSpeed
    ));

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/animated-panels.png",
        tileSize, {0,0}, panelSpacing,
        60, 72,
        panelAnimationSpeed
    ));

    //spikes

    spikeRenderData.push_back(LoadRenderData(
        "assets/tiles/spikes.png", 
        tileSize, {0,0},
        1,
        0,1
    ));

    spikeSmallRenderData.push_back(LoadRenderData(
        "assets/tiles/spikes.png", 
        tileSize, {0,0},
        1,
        1,2
    ));

    spikeDobuleRenderData.push_back(LoadRenderData(
        "assets/tiles/spikes.png", 
        tileSize, {0,0},
        1,
        2,3
    ));

    spikeBallRenderData.push_back(LoadRenderData(
        "assets/tiles/spikes.png", 
        tileSize, {0,0},
        1,
        3,4
    ));

    //treadmills

    //decoration

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/shadows.png",
        tileSize,{0,0},
        1,
        0,4
    ));

    //wind tiles

    windRenderData.push_back(LoadRenderData(
        "assets/tiles/wind-up.png", 
        tileSize,
        {0,0},
        3
    ));

    //water

    waterRenderData.push_back(LoadRenderData(
        "assets/tiles/water.png",
        tileSize,
        {0,0},
        3
    ));

    //platforms

    //enemies

    dummyRenderData.push_back(LoadRenderData("assets/enemies/chibi-dummy.png", {8,12}));
    
    yuukaRenderData.push_back(LoadRenderData(
        "assets/enemies/yuuka-gbc-sprtiesheet.png",
        {22,22},
        {-6,-8},
        1, 0, 0,
        10.0f
    ));

    //enemies' weapons

    yuukaWeaponRenderData.push_back(LoadRenderData(
        "assets/enemies/yuuka-weapon-holo.png",
        {10,4},
        {50,7}
    ));

    //player

    momoiRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-gbc-spritesheet.png",
        {13.0f, 24.0f},
        {0.0f, -11.0f},
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
        "assets/characters/midori-gbc-spritesheet.png",
        {13.0f, 24.0f},
        {0.0f, -11.0f},
        1,0,0, 
        10.0f
    ));

    yuzuRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-gbc-spritesheet.png",
        {17.0f, 21.0f},
        {-8.0f, -5.0f},
        1, 0, 0,
        10.0f
    ));

    yuzuRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-battle-gbc-spritesheet.png",
        {17.0f, 24.0f},
        {-8.0f, -12.0f},
        1, 0, 0,
        10.0f
    ));

    arisRenderData.push_back(LoadRenderData(
        "assets/characters/aris-gbc-spritesheet.png",
        {16.0f, 21.0f},
        {-6.0f, -6.0f},
        1, 0, 0,
        10.0f
    ));

    //player's weapons

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-weapon-holo.png",
        {10.0f, 5.0f},
        {50.0f, 7.0f}
    ));

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-chaquena-weapon-holo.png",
        {5.0f, 7.0f},
        {39.0f, 7.0f}
    ));

    midoriWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/midori-weapon-holo.png",
        {11.0f, 5.0f},
        {52.0f, 7.0f}
    ));

    yuzuWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-weapon-holo.png",
        {10.0f, 5.0f},
        {52.0f, 7.0f}
    ));

    arisWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/aris-weapon-holo.png",
        {15.0f, 4.0f},
        {60.0f, 7.0f}
    ));

    //UI

    uiBackground = LoadTexture("assets/ui/ui-background.png");

    portraits.push_back(LoadRenderData(
        "assets/ui/yuzu-portrait.png",
        {32,32}
    ));

    uiElements.push_back(LoadRenderData(
        "assets/ui/health-point.png", tileSize
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

    CleanUp(decoRenderData);

    //spikes

    CleanUp(spikeRenderData);

    CleanUp(spikeDobuleRenderData);

    CleanUp(spikeSmallRenderData);

    //platforms

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
