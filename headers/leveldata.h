#pragma once 

#include "raylib.h"
#include "entity.h"

#include "drawing.h"

#include <vector>

static constexpr Color PLAYER_SPAWN = Color{251,242,54,255};

static constexpr Color GOAL = Color{0,255,0,255};

static constexpr Color HORIZONTAL_MOVING_PLATFORM = Color{0,100,255,255};

static constexpr Color VERTICAL_MOVING_PLATFORM = Color{255,100,0,255};

static constexpr Color PLATFORM_STOP = Color{255,100,255,255};

static constexpr Color TRAMPOLINE = Color{150,150,200,255};

static constexpr Color GRAVITY_CHANGER = Color{150,200,200,255};

static constexpr Color TREADMILL_RIGHT = Color{0,0,255,255};

static constexpr Color TREADMILL_LEFT = Color{0,0,155,255};

static constexpr Color FALLING_PLATFORM = Color{200,100,0,255};

static constexpr Color ONE_WAY_UP = Color{0, 255,255,255};

static constexpr Color ONE_WAY_DOWN = Color{0, 200,255,255};

static constexpr Color ONE_WAY_RIGHT = Color{0, 200,190,255};

static constexpr Color ONE_WAY_LEFT = Color{0, 230,160,255};

static constexpr Color DISAPPEARING_PLATFORM = Color{200,100,140,255};

static constexpr Color SPIKE = Color{220,45,50,255};

static constexpr int ROWS = 70;
static constexpr int COLS = 70;

static constexpr int DEFAULT_INVALID_INDEX = -1;

static constexpr int gridSize = 48;

static constexpr int tileScale = 3;

struct TileRange
{
    int startX = 0;
    int startY = 0;

    int endX = ROWS;
    int endY = COLS;
};

enum class TileType
{
    VOID,
    
    TILE_START,

    SOLID,

    SPIKE,

    ONE_WAY_UP,
    ONE_WAY_DOWN,
    ONE_WAY_RIGHT,
    ONE_WAY_LEFT,

    TREADMILL_RIGHT,
    TREADMILL_LEFT,

    TRAMPOLINE,
    
    GRAVITY_CHANGER,

    TILE_END,

    PLATFORM_START,

    HORIZONALT_MOVING_PLATFORM,
    VERTICAL_MOVING_PLATFORM,
    FALLING_PLATFORM,
    DISAPPEARING_PLATFORM,

    PLATFORM_END,

    MISC_START,

    GOAL,

    PLATFORM_STOP,

    //non real tiles
    LOGIC_START,

    PLAYER_SPAWN,

    LOGIC_END,

    MISC_END,

    COUNT
};

inline bool IsTypeInvalid(TileType type)
{
    switch (type)
    {
    case TileType::TILE_START:

    case TileType::TILE_END:

    case TileType::PLATFORM_START:

    case TileType::PLATFORM_END:

    case TileType::MISC_START:

    case TileType::MISC_END:

    case TileType::LOGIC_START:

    case TileType::LOGIC_END:

    case TileType::COUNT:

        return true;
    
    default: return false;
    }
}

struct TileTypeList
{
    TileType type = TileType::VOID;
    Color color = BLANK;
    const char* name = "BLANK";
};

const std::vector<TileTypeList> TILE_TYPE_LIST = {
    {TileType::VOID, BLANK, "BLANK"},
    {TileType::SOLID, BLACK, "SOLID"},
    {TileType::GOAL, GOAL, "GOAL"},
    {TileType::PLATFORM_STOP, PLATFORM_STOP, "PLATFORM_STOP"},
    {TileType::TRAMPOLINE, TRAMPOLINE, "TRAMPOLINE"},
    {TileType::GRAVITY_CHANGER, GRAVITY_CHANGER, "GRAVITY_CHANGER"},
    {TileType::TREADMILL_RIGHT, TREADMILL_RIGHT, "TREADMILL_RIGHT"},
    {TileType::TREADMILL_LEFT, TREADMILL_LEFT, "TREADMILL_LEFT"},
    {TileType::ONE_WAY_UP, ONE_WAY_UP, "ONE_WAY_UP"},
    {TileType::ONE_WAY_DOWN, ONE_WAY_DOWN, "ONE_WAY_DOWN"},
    {TileType::ONE_WAY_RIGHT, ONE_WAY_RIGHT, "ONE_WAY_RIGHT"},
    {TileType::ONE_WAY_LEFT, ONE_WAY_LEFT, "ONE_WAY_LEFT"},
    {TileType::SPIKE, SPIKE, "SPIKE"},
    {TileType::HORIZONALT_MOVING_PLATFORM, HORIZONTAL_MOVING_PLATFORM, "HORIZONALT_MOVING_PLATFORM"},
    {TileType::VERTICAL_MOVING_PLATFORM, VERTICAL_MOVING_PLATFORM, "VERTICAL_MOVING_PLATFORM"},
    {TileType::FALLING_PLATFORM, FALLING_PLATFORM, "FALLING_PLATFORM"},
    {TileType::DISAPPEARING_PLATFORM, DISAPPEARING_PLATFORM, "DISAPPEARING_PLATFORM"},
    {TileType::PLAYER_SPAWN, PLAYER_SPAWN, "PLAYER_SPAWN"},
};

struct Tile
{
    TileType type = TileType::VOID;
    int textureIndex = DEFAULT_INVALID_INDEX;

    int variantIndex = 0;
    
    GameObject* gameObj = nullptr;
};

inline bool IsColorOf(Color colorA, Color colorB)
{
    return colorA.r == colorB.r && 
    colorA.g == colorB.g && 
    colorA.b == colorB.b &&
    colorA.a == colorB.a;
}

inline bool IsTileEmpty(int i, int j, Tile(&levelTiles)[ROWS][COLS], TileType emptyType = TileType::VOID)
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS) return true;

    return levelTiles[i][j].type == emptyType;
}

inline bool IsTileEmptyInverted(int i, int j, Tile(&levelTiles)[ROWS][COLS], TileType emptyType = TileType::VOID)
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS) return true;

    return levelTiles[i][j].type != emptyType;
}

inline TileRange CalculateTileRange(int x, int y, int range)
{
    int gridX = x / gridSize;
    int gridY = y / gridSize;

    TileRange rangeTiles = {};

    rangeTiles.startX = fmaxf(0, gridX - range);
    rangeTiles.endX = fminf(ROWS - 1, gridX + range);

    rangeTiles.startY = fmaxf(0, gridY - range);
    rangeTiles.endY = fminf(COLS - 1, gridY + range);

    return rangeTiles;
}

inline Color GetTileColor(TileType type)
{
    for(int i = 0; i < TILE_TYPE_LIST.size(); i++)
    {
        if(TILE_TYPE_LIST[i].type == type)
        {
            return TILE_TYPE_LIST[i].color;
        }
    }
    
    return BLANK;
}

inline const char* GetTileTypeText(TileType type)
{
    for(int i = 0; i < TILE_TYPE_LIST.size(); i++)
    {
        if(TILE_TYPE_LIST[i].type == type)
        {
            return TILE_TYPE_LIST[i].name;
        }
    }

    return "";
}

//textures

extern std::vector<SpriteRenderData> solidTilesRenderData;

extern std::vector<SpriteRenderData> spikesRenderData;

extern std::vector<SpriteRenderData> treadmillRenderData_Right;

extern std::vector<SpriteRenderData> treadmillRenderData_Left;

void LoadAssets();

void UnloadAssets();

inline std::vector<SpriteRenderData>* GetActiveRenderDataList(TileType type)
{
    switch (type)
    {
    case TileType::SOLID:return &solidTilesRenderData;

    case TileType::TREADMILL_RIGHT: return &treadmillRenderData_Right;
    case TileType::TREADMILL_LEFT: return &treadmillRenderData_Left;

    case TileType::SPIKE: return &spikesRenderData;

    default: return nullptr;
    } 
}

inline SpriteRenderData* GetActiveRenderData(TileType type, int index = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetActiveRenderDataList(type);

    if(activeRenderData && index >= 0 && index < (int)activeRenderData->size())
    {
        return &activeRenderData->at(index);
    }

    return nullptr;
}
