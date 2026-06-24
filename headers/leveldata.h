#pragma once 

#include "definitions.h"

#include "entity.h"

#include "drawing.h"

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

    ONE_WAY,

    TREADMILL_RIGHT,
    TREADMILL_LEFT,

    TRAMPOLINE,
    
    GRAVITY_CHANGER,

    WIND,

    WATER,

    LADDER,

    TILE_END,

    SPIKE_START,

    SPIKE,
    SPIKE_DOUBLE,
    SPIKE_SMALL,
    SPIKE_BALL,

    SPIKE_END,

    MISC_START,

    GOAL,

    PLATFORM_STOP,

    PLAYER_SPAWN, //not a real tile

    MISC_END,

    DECO,

    //platforms
    PLATFORM_START,

    HORIZONALT_MOVING_PLATFORM,
    VERTICAL_MOVING_PLATFORM,
    FALLING_PLATFORM,
    DISAPPEARING_PLATFORM,

    VERTICAL_MOVING_SPIKE,
    HORIZONTAL_MOVING_SPIKE,

    ROTATING_SPIKE_SINGLE,
    ROTATING_SPIKE_DOUBLE,

    PLATFORM_END,

    ENEMY_START,

    ENEMY_DUMMY,

    ENEMY_YUUKA,

    ENEMY_END,

    COUNT
};

enum class PlatformType
{
    NONE,
    
    FALLING,
    DISAPPEARING,

    ROTATING_SPIKE_SINGLE,
    ROTATING_SPIKE_DOUBLE,

    MOVING_START,

    MOVING_X,

    MOVING_HORIZONTAL,
    MOVING_SPIKE_HORIZONTAL,
    
    MOVING_Y,

    MOVING_VERTICAL,
    MOVING_SPIKE_VERTICAL,

    MOVING_END
};

enum class EnemyType
{
    DUMMY,
    YUUKA
};

enum class Character
{
    MOMOI,
    MIDORI,
    YUZU,
    YUZU_BATTLE,
    ARIS,
    MOMOI_CHAQUENA
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

    case TileType::ENEMY_START:

    case TileType::ENEMY_END:

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

    {TileType::WIND, WHITE, "WIND"},

    {TileType::WATER, SKYBLUE, "WATER"},

    {TileType::LADDER, LADDER, "LADDER"},
    
    {TileType::TREADMILL_RIGHT, TREADMILL_RIGHT, "TREADMILL_RIGHT"},
    {TileType::TREADMILL_LEFT, TREADMILL_LEFT, "TREADMILL_LEFT"},

    {TileType::ONE_WAY, ONE_WAY, "ONE_WAY"},

    {TileType::SPIKE, SPIKE, "SPIKE"},
    {TileType::SPIKE_DOUBLE, SPIKE, "SPIKE_DOUBLE"},
    {TileType::SPIKE_SMALL, SPIKE, "SPIKE_SMALL"},
    {TileType::SPIKE_BALL, SPIKE, "SPIKE_BALL"},

    {TileType::HORIZONALT_MOVING_PLATFORM, HORIZONTAL_MOVING_PLATFORM, "HORIZONALT_MOVING_PLATFORM"},
    {TileType::VERTICAL_MOVING_PLATFORM, VERTICAL_MOVING_PLATFORM, "VERTICAL_MOVING_PLATFORM"},
    {TileType::FALLING_PLATFORM, FALLING_PLATFORM, "FALLING_PLATFORM"},
    {TileType::DISAPPEARING_PLATFORM, DISAPPEARING_PLATFORM, "DISAPPEARING_PLATFORM"},

    {TileType::VERTICAL_MOVING_SPIKE, SPIKE, "VERTICAL_MOVING_SPIKE"},

    {TileType::HORIZONTAL_MOVING_SPIKE, SPIKE, "HORIZONTAL_MOVING_SPIKE"},

    {TileType::ROTATING_SPIKE_SINGLE, SPIKE, "ROTATING_SPIKE_SINGLE"},

    {TileType::ROTATING_SPIKE_DOUBLE, SPIKE, "ROTATING_SPIKE_DOUBLE"},

    {TileType::PLAYER_SPAWN, PLAYER_SPAWN, "PLAYER_SPAWN"},

    {TileType::DECO, DECO, "DECO"},

    //enemies / bosses
    {TileType::ENEMY_DUMMY, ENEMY_DUMMY, "ENEMY_DUMMY"},

    {TileType::ENEMY_YUUKA, ENEMY_YUUKA, "ENEMY_YUUKA"},
};

enum class NeighborDirection
{
    UP_LEFT,
    UP,
    UP_RIGHT,

    RIGHT,
    DOWN_RIGHT,
    DOWN,

    DOWN_LEFT,
    LEFT
};

struct Tile
{
    TileType type = TileType::VOID;

    int textureIndex = 0;

    int variantIndex = 0;
    
    bool isJumpTrigger = false;

    GameObject gameObj = {};

    TileType neighborsTypes[8] = {TileType::VOID};

    inline TileType GetNeighborType(NeighborDirection direction)
    {
        return neighborsTypes[(int)direction];
    }
};

struct Room
{
    Rectangle aabb = {};
};

inline bool IsColorOf(Color colorA, Color colorB)
{
    return colorA.r == colorB.r && 
    colorA.g == colorB.g && 
    colorA.b == colorB.b &&
    colorA.a == colorB.a;
}

inline bool IsTileEqual(int l, int i, int j, const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS || l < 0 || l >= LAYERS) return true;

    return levelTiles[l][i][j].type == tileType;
}

inline bool IsTileEqual(const int(&matrixPosition)[3], const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    return IsTileEqual(matrixPosition[0], matrixPosition[1], matrixPosition[2], levelTiles, tileType);
}

inline bool IsTileNotEqual(int l, int i, int j, const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS || l < 0 || l >= LAYERS) return true;

    return levelTiles[l][i][j].type != tileType;
}

inline bool IsTileNotEqual(const int(&matrixPosition)[3], const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    return IsTileNotEqual(matrixPosition[0], matrixPosition[1], matrixPosition[2], levelTiles, tileType);
}

inline TileType GetTileType(int l, int i, int j, const Tile(&levelTiles)[LAYERS][COLS][ROWS])
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS || l < 0 || l >= LAYERS) return TileType::VOID;

    return levelTiles[l][i][j].type;
}

inline TileType GetTileType(const int(&matrixPosition)[3], const Tile(&levelTiles)[LAYERS][COLS][ROWS])
{
    return GetTileType(matrixPosition[0], matrixPosition[1], matrixPosition[2], levelTiles);
}

inline TileRange CalculateTileRange(
    int x, int y, int range, 
    int minX = 0, int minY = 0, 
    int maxX = COLS - 1, int maxY = ROWS - 1
)
{
    int gridX = x / GRID_SIZE;
    int gridY = y / GRID_SIZE;

    TileRange rangeTiles = {};

    rangeTiles.startX = fmaxf(minX, gridX - range);
    rangeTiles.endX = fminf(maxX, gridX + range);

    rangeTiles.startY = fmaxf(minY, gridY - range);
    rangeTiles.endY = fminf(maxY, gridY + range);

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
    return {i * GRID_SIZE + GRID_SIZE * 0.5f, j * GRID_SIZE + GRID_SIZE * 0.5f};
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
extern std::vector<SpriteRenderData> windRenderData;

extern std::vector<SpriteRenderData> windRenderData_Down;

extern std::vector<SpriteRenderData> windRenderData_Left;

extern std::vector<SpriteRenderData> windRenderData_Right;

//water

extern std::vector<SpriteRenderData> waterRenderData;

//platforms

extern std::vector<SpriteRenderData> movingPlatformRenderData_Vertical;

//enemies

extern std::vector<SpriteRenderData> dummyRenderData;

extern std::vector<SpriteRenderData> yuukaRenderData;

//enemy's weapons

extern std::vector<SpriteRenderData> yuukaWeaponRenderData;

//player

extern std::vector<SpriteRenderData> momoiRenderData;
extern std::vector<SpriteRenderData> midoriRenderData;

extern std::vector<SpriteRenderData> yuzuRenderData;
extern std::vector<SpriteRenderData> arisRenderData;

//player's weapons

extern std::vector<SpriteRenderData> momoiWeaponRenderData;

extern std::vector<SpriteRenderData> midoriWeaponRenderData;

extern std::vector<SpriteRenderData> yuzuWeaponRenderData;
extern std::vector<SpriteRenderData> arisWeaponRenderData;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern SpriteRenderData LoadRenderData(const char* path, Vector2 frameSize, Vector2 offset = {0,0}, int spacing = 1, int atlasStartFrame = 0, int atlasEndFrame = 0, float animationSpeed = 5.0f);

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
    
    case TileType::WIND: return &windRenderData;

    //water

    case TileType::WATER: return &waterRenderData;
    
    //platforms

    case TileType::VERTICAL_MOVING_PLATFORM: return &movingPlatformRenderData_Vertical;

    //enemies 

    case TileType::ENEMY_DUMMY: return &dummyRenderData;

    case TileType::ENEMY_YUUKA: return &yuukaRenderData;

    default: return nullptr;
    }
}

inline SpriteRenderData* GetTileActiveRenderData(TileType type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetTileActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
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

inline SpriteRenderData* GetPlatformActiveRenderData(PlatformType type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetPlatformActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
    }

    return nullptr;
}

inline std::vector<SpriteRenderData>* GetEnemyActiveRenderDataList(EnemyType type)
{
    switch (type)
    {
    case EnemyType::DUMMY: return &dummyRenderData;
    
    case EnemyType::YUUKA: return &yuukaRenderData;

    default: return nullptr;
    }
}

inline SpriteRenderData* GetEnemyActiveRenderData(EnemyType type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetEnemyActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
    }

    return nullptr;
}

inline std::vector<SpriteRenderData>* GetEnemyWeaponActiveRenderDataList(EnemyType type)
{
    switch (type)
    {
    
    case EnemyType::YUUKA: return &yuukaWeaponRenderData;

    default: return nullptr;
    }
}

inline SpriteRenderData* GetEnemyWeaponActiveRenderData(EnemyType type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetEnemyWeaponActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
    }

    return nullptr;
}

inline std::vector<SpriteRenderData>* GetPlayerActiveRenderDataList(Character type)
{
    switch (type)
    {
    
    case Character::MOMOI: 
    case Character::MOMOI_CHAQUENA:
        return &momoiRenderData;

    case Character::MIDORI: return &midoriRenderData;

    case Character::YUZU:
    case Character::YUZU_BATTLE:
        return &yuzuRenderData;

    case Character::ARIS: return &arisRenderData;

    default: return nullptr;
    }
}

inline SpriteRenderData* GetPlayerActiveRenderData(Character type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetPlayerActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
    }

    return nullptr;
}

inline std::vector<SpriteRenderData>* GetPlayerWeaponActiveRenderDataList(Character type)
{
    switch (type)
    {
    
    case Character::MOMOI:
    case Character::MOMOI_CHAQUENA:
        return &momoiWeaponRenderData;

    case Character::MIDORI: return &midoriWeaponRenderData;

    case Character::YUZU:
    case Character::YUZU_BATTLE:
        return &yuzuWeaponRenderData;

    case Character::ARIS: return &arisWeaponRenderData;

    default: return nullptr;
    }
}

inline SpriteRenderData* GetPlayerWeaponActiveRenderData(Character type, int variant = 0)
{
    std::vector<SpriteRenderData>* activeRenderData = GetPlayerWeaponActiveRenderDataList(type);

    if(activeRenderData && variant >= 0 && variant < (int)activeRenderData->size())
    {
        return &activeRenderData->at(variant);
    }

    return nullptr;
}

inline void LoadLevelData(const char* levelPath, Tile(&destination)[LAYERS][COLS][ROWS], const char* roomPath, std::vector<Room>& rooms)
{
    int dataSize = 0;

    unsigned char* fileData = LoadFileData(levelPath, &dataSize);

    if(fileData == nullptr) return;

    if(dataSize != (LAYERS * ROWS * COLS * sizeof(Tile))) return;

    memcpy(destination, fileData, dataSize);
    
    UnloadFileData(fileData);

    int roomDataSize = 0;

    unsigned char* roomFileData = LoadFileData(roomPath, &roomDataSize);

    if(roomFileData == nullptr) return;

    int roomCount = roomDataSize / sizeof(Room);

    if(roomCount > 0)
    {
        rooms.resize(roomCount);

        memcpy(rooms.data(), roomFileData, roomDataSize);
    }
    else
    {
        rooms.clear();
    }

    UnloadFileData(roomFileData);
}