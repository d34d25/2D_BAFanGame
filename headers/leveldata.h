#pragma once 

#include "definitions.h"

#include "entity.h"

#include "drawing.h"

#include "platform.h"

#include <vector>

#include <cstring>

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

    ONE_WAY_UP,
    ONE_WAY_DOWN,
    ONE_WAY_RIGHT,
    ONE_WAY_LEFT,

    TREADMILL_RIGHT,
    TREADMILL_LEFT,

    TRAMPOLINE,
    
    GRAVITY_CHANGER,

    WIND_UP,
    WIND_DOWN,
    WIND_LEFT,
    WIND_RIGHT,

    WATER,

    TILE_END,

    SPIKE_START,

    SPIKE,
    SPIKE_DOUBLE,
    SPIKE_SMALL,

    SPIKE_END,

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

    DECO,

    COUNT
};

inline bool IsTypeInvalid(TileType type)
{
    switch (type)
    {
    case TileType::TILE_START:

    case TileType::TILE_END:

    case TileType::SPIKE_START:

    case TileType::SPIKE_END:

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

    {TileType::WIND_UP, WHITE, "WIND_UP"},
    {TileType::WIND_DOWN, WHITE, "WIND_DOWN"},
    {TileType::WIND_LEFT, WHITE, "WIND_LEFT"},
    {TileType::WIND_RIGHT, WHITE, "WIND_RIGHT"},

    {TileType::WATER, SKYBLUE, "WATER"},
    
    {TileType::TREADMILL_RIGHT, TREADMILL_RIGHT, "TREADMILL_RIGHT"},
    {TileType::TREADMILL_LEFT, TREADMILL_LEFT, "TREADMILL_LEFT"},

    {TileType::ONE_WAY_UP, ONE_WAY_UP, "ONE_WAY_UP"},
    {TileType::ONE_WAY_DOWN, ONE_WAY_DOWN, "ONE_WAY_DOWN"},
    {TileType::ONE_WAY_RIGHT, ONE_WAY_RIGHT, "ONE_WAY_RIGHT"},
    {TileType::ONE_WAY_LEFT, ONE_WAY_LEFT, "ONE_WAY_LEFT"},

    {TileType::SPIKE, SPIKE, "SPIKE"},
    {TileType::SPIKE_DOUBLE, SPIKE, "SPIKE_DOUBLE"},
    {TileType::SPIKE_SMALL, SPIKE, "SPIKE_SMALL"},

    {TileType::HORIZONALT_MOVING_PLATFORM, HORIZONTAL_MOVING_PLATFORM, "HORIZONALT_MOVING_PLATFORM"},
    {TileType::VERTICAL_MOVING_PLATFORM, VERTICAL_MOVING_PLATFORM, "VERTICAL_MOVING_PLATFORM"},
    {TileType::FALLING_PLATFORM, FALLING_PLATFORM, "FALLING_PLATFORM"},
    {TileType::DISAPPEARING_PLATFORM, DISAPPEARING_PLATFORM, "DISAPPEARING_PLATFORM"},

    {TileType::PLAYER_SPAWN, PLAYER_SPAWN, "PLAYER_SPAWN"},

    {TileType::DECO, DECO, "DECO"},
};

struct Tile
{
    TileType type = TileType::VOID;

    int textureIndex = 0;

    int variantIndex = 0;
    
    GameObject gameObj = {};
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

inline Vector2 GetTileCenter(int i, int j)
{
    return {i * gridSize + gridSize * 0.5f, j * gridSize + gridSize * 0.5f};
}

//textures


//tiles

extern std::vector<SpriteRenderData> solidTilesRenderData;

extern std::vector<SpriteRenderData> treadmillRenderData_Right;

extern std::vector<SpriteRenderData> treadmillRenderData_Left;

extern std::vector<SpriteRenderData> decoRenderData;

//spikes

extern std::vector<SpriteRenderData> spikesRenderData;

extern std::vector<SpriteRenderData> spikesDobuleRenderData;

extern std::vector<SpriteRenderData> spikesSmallRenderData;


//wind tiles
extern std::vector<SpriteRenderData> windRenderData_Up;

extern std::vector<SpriteRenderData> windRenderData_Down;

extern std::vector<SpriteRenderData> windRenderData_Left;

extern std::vector<SpriteRenderData> windRenderData_Right;

//water

extern std::vector<SpriteRenderData> waterRenderData;

//platforms

extern std::vector<SpriteRenderData> movingPlatformRenderData_Vertical;

void LoadAssets();

void UnloadAssets();

inline std::vector<SpriteRenderData>* GetTileActiveRenderDataList(TileType type)
{
    switch (type)
    {
    case TileType::SOLID:return &solidTilesRenderData;

    case TileType::TREADMILL_RIGHT: return &treadmillRenderData_Right;
    case TileType::TREADMILL_LEFT: return &treadmillRenderData_Left;

    //spikes

    case TileType::SPIKE: return &spikesRenderData;

    case TileType::SPIKE_DOUBLE: return &spikesDobuleRenderData;

    case TileType::SPIKE_SMALL: return &spikesSmallRenderData;

    //deco

    case TileType::DECO: return &decoRenderData;

    //wind tiles
    
    case TileType::WIND_UP: return &windRenderData_Up;

    case TileType::WIND_DOWN: return &windRenderData_Down;

    case TileType::WIND_LEFT: return &windRenderData_Left;

    case TileType::WIND_RIGHT: return &windRenderData_Right;

    //water

    case TileType::WATER: return &waterRenderData;
    
    //platforms (only used by the editor)

    case TileType::VERTICAL_MOVING_PLATFORM: return &movingPlatformRenderData_Vertical;

    default: return nullptr;
    } 
}

inline SpriteRenderData* GetTileActiveRenderData(TileType type, int index = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetTileActiveRenderDataList(type);

    if(activeRenderData && index >= 0 && index < (int)activeRenderData->size())
    {
        return &activeRenderData->at(index);
    }

    return nullptr;
}


inline std::vector<SpriteRenderData>* GetPlatformActiveRenderDataList(PlatformType type)
{
    switch (type)
    {
    case PlatformType::MOVING_VERTICAL: return &movingPlatformRenderData_Vertical;
    default: return nullptr;
    }
}

inline SpriteRenderData* GetPlatformActiveRenderData(PlatformType type, int index = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetPlatformActiveRenderDataList(type);

    if(activeRenderData && index >= 0 && index < (int)activeRenderData->size())
    {
        return &activeRenderData->at(index);
    }

    return nullptr;
}

inline void LoadLevelData(const char* levelPath, Tile(&destination)[ROWS][COLS])
{
    int dataSize = 0;

    unsigned char* fileData = LoadFileData(levelPath, &dataSize);

    if(fileData == nullptr) return;

    if(dataSize != (ROWS * COLS * sizeof(Tile))) return;

    memcpy(destination, fileData, dataSize);
    
    UnloadFileData(fileData);
}