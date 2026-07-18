#include "leveldata.h"

#include <map>

#include <string>

static std::map<std::string, Texture2D> textureCache = {};

std::vector<std::array<Color, MAX_PALETTE_COLS>> spritePalettes = {};

std::vector<std::array<Color, MAX_PALETTE_COLS>> environmentPalettes = {};

//shader

Shader paletteShader = {0};

int paletteLoc = -1;

int lastPaletteIndex = -1;

const std::vector<std::array<Color, MAX_PALETTE_COLS>>* lastPaletteList = nullptr;

//tiles
std::vector<SpriteRenderData> solidTilesRenderData = {};

std::vector<SpriteRenderData> treadmillRenderData_Right = {};

std::vector<SpriteRenderData> treadmillRenderData_Left = {};

std::vector<SpriteRenderData> decoRenderData = {};

std::vector<SpriteRenderData> ladderRenderData = {};

std::vector<SpriteRenderData> windRenderData = {};

std::vector<SpriteRenderData> waterRenderData = {};

std::vector<SpriteRenderData> trampolineRenderData = {};

std::vector<SpriteRenderData> oneWayRenderData = {};

std::vector<SpriteRenderData> gravityChagerRenderData = {};

//spikes

std::vector<SpriteRenderData> spikeRenderData = {};

std::vector<SpriteRenderData> spikeDobuleRenderData = {};

std::vector<SpriteRenderData> spikeSmallRenderData = {};

std::vector<SpriteRenderData> spikeBallRenderData = {};

std::vector<SpriteRenderData> movingSpikeRenderData = {};

//platforms

std::vector<SpriteRenderData> verticalMovingPlatform_RenderData = {};

std::vector<SpriteRenderData> horizontalMovingPlatform_RenderData = {};

std::vector<SpriteRenderData> fallingPlatform_RenderData = {};

std::vector<SpriteRenderData> disappearingPlatform_RenderData = {};

//enemies

std::vector<SpriteRenderData> dummyRenderData = {};

std::vector<SpriteRenderData> amasDroneRenderData = {};

std::vector<SpriteRenderData> sweeperARenderData = {};

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

std::vector<SpriteRenderData> portraits = {};

std::vector<SpriteRenderData> uiElements = {};

//backgrounds

std::vector<Texture2D> backgrounds = {};

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
    spritePalettes = LoadPalette("assets/sprite-pallet-set.png");

    environmentPalettes = LoadPalette("assets/environment-pallet-set.png");

    paletteShader = LoadShader(nullptr, "shaders/palette.fs");

    paletteLoc = GetShaderLocation(paletteShader, "uPalette");
   
    Vector2 tileSize = {TILE_SIZE,TILE_SIZE};

    backgrounds.push_back(LoadTexture("assets/backgrounds/testBackground.png"));

    //tiles

    solidTilesRenderData.push_back(LoadRenderData(
        "assets/tiles/solid/brave-aris-tiles.png", tileSize
    ));

    ladderRenderData.push_back(LoadRenderData("assets/tiles/ladder.png", tileSize));

    //decoration

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/gdd-cat-gbc.png",
        {35,29},{0,0},
        1
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/windows.png",
        tileSize,{0,0},
        1
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/2x3Windows.png",
        {TILE_SIZE * 2, TILE_SIZE * 3},{TILE_SIZE * 0.5f, TILE_SIZE},
        1
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/windows-2.png",
        tileSize,{0,0},
        1
    ));

    float treeOffset = (-TILE_SIZE * 0.5f) - 1;

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/tree-background-1a.png", {32,80}, {0, treeOffset}
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/tree-background-1b.png", {32,80}, {0, treeOffset}
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/tree-background-2a.png", {32,80}, {0, treeOffset}
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/tree-background-2b.png", {32,80}, {0, treeOffset}
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/sky-background-gradient-a.png", {80,32}, {TILE_SIZE * 0.5f, TILE_SIZE * 0.5f},
        1
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/2x3Windows-2.png",
        {TILE_SIZE * 2, TILE_SIZE * 3},{TILE_SIZE * 0.5f, TILE_SIZE},
        1
    ));

    decoRenderData.push_back(LoadRenderData(
        "assets/tiles/deco/shadows.png",
        tileSize,{0,0}
    ));


    //rest of tiles

    oneWayRenderData.push_back(LoadRenderData(
        "assets/tiles/one-way.png", tileSize, {0,0},
        2
    ));

    trampolineRenderData.push_back(LoadRenderData(
        "assets/tiles/trampoline.png", tileSize
    ));

    gravityChagerRenderData.push_back(LoadRenderData(
        "assets/tiles/gravity-changer.png", tileSize, {0,0},
        6
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

    movingSpikeRenderData.push_back(LoadRenderData(
        "assets/tiles/spikes.png", tileSize, {0,0}, 1,
        4, 5
    ));

    //treadmills

    treadmillRenderData_Right.push_back(LoadRenderData(
        "assets/tiles/treadmills.png",
        tileSize, {0,0}, 2,
        0, 6
    ));

    treadmillRenderData_Left.push_back(LoadRenderData(
        "assets/tiles/treadmills.png",
        tileSize, {0,0}, 2,
        6, 12
    ));

    //wind tiles

    windRenderData.push_back(LoadRenderData(
        "assets/tiles/wind.png", 
        tileSize,
        {0,0},
        2
    ));

    //water

    waterRenderData.push_back(LoadRenderData(
        "assets/tiles/water.png",
        tileSize,
        {0,0},
        2
    ));

    //platforms

    verticalMovingPlatform_RenderData.push_back(LoadRenderData(
        "assets/platforms/vertical-moving-platform.png", 
        {22, 3}, {0,0}
    ));

    horizontalMovingPlatform_RenderData.push_back(LoadRenderData(
        "assets/platforms/horizontal-moving-platform.png", 
        {22, 3}, {0,0},
        2
    ));

    fallingPlatform_RenderData.push_back(LoadRenderData(
        "assets/platforms/falling-platform.png", 
        tileSize, {0,0}
    ));

    disappearingPlatform_RenderData.push_back(LoadRenderData(
        "assets/platforms/disappearing-platform.png", 
        tileSize, {0,0}
    ));
    

    //enemies

    dummyRenderData.push_back(LoadRenderData("assets/enemies/chibi-dummy.png", {8,12}, {0,0}));

    amasDroneRenderData.push_back(LoadRenderData(
        "assets/enemies/amas-drone.png", {12,6},
        {0,0}
    ));
    
    sweeperARenderData.push_back(LoadRenderData(
        "assets/enemies/sweeper-1.png", {9,10},
        {0,0}, 1, 0,0, 10.0f
    ));

    yuukaRenderData.push_back(LoadRenderData(
        "assets/enemies/yuuka-gbc-sprtiesheet.png",
        {22,22},
        {-1,-2},
        1, 0, 0,
        10.0f
    ));

    //enemies' weapons

    yuukaWeaponRenderData.push_back(LoadRenderData(
        "assets/enemies/yuuka-weapon-holo.png",
        {10,4},
        {13,2}
    ));

    //player

    momoiRenderData.push_back(LoadRenderData(
        "assets/characters/minimalist-momoi-midori.png",
        {13.0f, 19.0f},
        {-1.0f, -1.0f},
        1,0,0,
        7.0f
    ));

    midoriRenderData.push_back(LoadRenderData(
        "assets/characters/minimalist-momoi-midori.png",
        {13.0f, 19.0f},
        {-1.0f, -1.0f},
        1,0,0,
        7.0f
    ));

    yuzuRenderData.push_back(LoadRenderData(
        "assets/characters/minimalist-yuzu.png",
        {15.0f, 16.0f},
        {0.0f, 0.0f},
        1,0,0,
        7.0f
    ));

    arisRenderData.push_back(LoadRenderData(
        "assets/characters/minimalist-aris.png",
        {16.0f, 17.0f},
        {-1.0f, 0.0f},
        1,0,0,
        7.0f
    ));

    //player's weapons

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-weapon-holo.png",
        {10.0f, 5.0f},
        {13.0f, 2.0f}
    ));

    momoiWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/momoi-chaquena-weapon-holo.png",
        {5.0f, 7.0f},
        {10.0f, 2.0f}
    ));

    midoriWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/midori-weapon-holo.png",
        {11.0f, 5.0f},
        {13.0f, 2.0f}
    ));

    yuzuWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/yuzu-weapon-holo.png",
        {10.0f, 5.0f},
        {13, 2}
    ));

    arisWeaponRenderData.push_back(LoadRenderData(
        "assets/characters/aris-weapon-holo.png",
        {15.0f, 4.0f},
        {15.0f, 2.0f}
    ));

    //UI

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

    CleanUp(windRenderData);

    CleanUp(waterRenderData);

    CleanUp(gravityChagerRenderData);

    CleanUp(oneWayRenderData);

    CleanUp(ladderRenderData);

    CleanUp(treadmillRenderData_Left);

    CleanUp(treadmillRenderData_Right);

    //spikes

    CleanUp(spikeRenderData);

    CleanUp(spikeDobuleRenderData);

    CleanUp(spikeSmallRenderData);

    //platforms

    CleanUp(movingSpikeRenderData);

    CleanUp(verticalMovingPlatform_RenderData);

    CleanUp(horizontalMovingPlatform_RenderData);

    CleanUp(fallingPlatform_RenderData);

    CleanUp(disappearingPlatform_RenderData);

    //enemies

    CleanUp(dummyRenderData);

    CleanUp(yuukaRenderData);

    CleanUp(amasDroneRenderData);

    CleanUp(sweeperARenderData);

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

    //UI
    CleanUp(portraits);

    CleanUp(uiElements);

    for(int i = 0; i < backgrounds.size(); i++)
    {
        UnloadTexture(backgrounds[i]);
    }

    backgrounds.clear();
    backgrounds.shrink_to_fit();

    UnloadShader(paletteShader);
}
