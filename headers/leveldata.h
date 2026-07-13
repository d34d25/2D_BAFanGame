#pragma once 

#include "definitions.h"

#include "entity.h"

#include "drawing.h"

#include <vector>

#include <array>

#include <cstring>

#include <string>

#include "rlgl.h"

struct TileRange
{
    int startX = 0;
    int startY = 0;

    int endX = ROWS;
    int endY = COLS;
};

enum struct TileType
{
    VOID,
    
    TILE_START,

    SOLID, //texture done

    ONE_WAY, //texture done

    TREADMILL_RIGHT, //texture done
    TREADMILL_LEFT, //texture done

    TRAMPOLINE, //texture done
    
    GRAVITY_CHANGER, //texture done

    WIND, //texture done

    WATER, //texture done

    LADDER, //texture done

    TILE_END,

    SPIKE_START,

    SPIKE, //texture done
    SPIKE_DOUBLE, //texture done
    SPIKE_SMALL, //texture done
    SPIKE_BALL, //texture done

    SPIKE_END,

    MISC_START,

    GOAL,

    PLATFORM_STOP,

    PLAYER_SPAWN, //not a real tile

    MISC_END,

    DECO, //texture done

    //platforms
    PLATFORM_START,

    HORIZONALT_MOVING_PLATFORM, //texture done
    VERTICAL_MOVING_PLATFORM, //texture done
    FALLING_PLATFORM,
    DISAPPEARING_PLATFORM,

    VERTICAL_MOVING_SPIKE, //texture done
    HORIZONTAL_MOVING_SPIKE, //texture done

    ROTATING_SPIKE_SINGLE, //texture done
    ROTATING_SPIKE_DOUBLE, //texture done

    PLATFORM_END,

    ENEMY_START,

    ENEMY_DUMMY, //texture done

    ENEMY_YUUKA, //texture done

    ENEMY_END,

    COUNT
};

enum struct PlatformType
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

enum struct EnemyType
{
    DUMMY,
    YUUKA
};

enum struct Character
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

    {TileType::SOLID, PINK, "SOLID"},

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
    std::vector<Hitbox> hitboxes = {};

    TileType neighborsTypes[8] = {TileType::VOID};

    TileType type = TileType::VOID;

    float treadmillVel = 0.0f;

    float angle = 0.0f;

    SpriteFlipData flipData = {};

    Direction direction = Direction::UP;

    int currentFrame = 0;

    int textureIndex = 0;

    int variantIndex = 0;

    int paletteIndex = 0;

    bool isMiddle = false;

    bool isJumpTrigger = false;

    bool canEntityCollidePhysically = false;

    bool canPlatformCollidePhysically = false;
};

struct Room
{
    Rectangle aabb = {};

    int currentPaletteIndex = 0;
    int currentColorIndex = 0;
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
    if(i < 0 || i >= COLS || j < 0 || j >= ROWS || l < 0 || l >= LAYERS) return true;

    return levelTiles[l][i][j].type == tileType;
}

inline bool IsTileEqual(const int(&matrixPosition)[3], const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    return IsTileEqual(matrixPosition[0], matrixPosition[1], matrixPosition[2], levelTiles, tileType);
}

inline bool IsTileNotEqual(int l, int i, int j, const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    if(i < 0 || i >= COLS || j < 0 || j >= ROWS || l < 0 || l >= LAYERS) return true;

    return levelTiles[l][i][j].type != tileType;
}

inline bool IsTileNotEqual(const int(&matrixPosition)[3], const Tile(&levelTiles)[LAYERS][COLS][ROWS], TileType tileType = TileType::VOID)
{
    return IsTileNotEqual(matrixPosition[0], matrixPosition[1], matrixPosition[2], levelTiles, tileType);
}

inline TileType GetTileType(int l, int i, int j, const Tile(&levelTiles)[LAYERS][COLS][ROWS])
{
    if(i < 0 || i >= COLS || j < 0 || j >= ROWS || l < 0 || l >= LAYERS) return TileType::VOID;

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
    int gridX = x / TILE_SIZE;
    int gridY = y / TILE_SIZE;

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
    return {i * TILE_SIZE + TILE_SIZE * 0.5f, j * TILE_SIZE + TILE_SIZE * 0.5f};
}

//palettes

extern std::vector<std::array<Color, MAX_PALETTE_COLS>> spritePalettes;

extern std::vector<std::array<Color, MAX_PALETTE_COLS>> environmentPalettes;

//shader

extern Shader paletteShader;

extern int paletteLoc;

extern int lastPaletteIndex;

extern const std::vector<std::array<Color, MAX_PALETTE_COLS>>* lastPaletteList;

//textures


//tiles

extern std::vector<SpriteRenderData> solidTilesRenderData;

extern std::vector<SpriteRenderData> treadmillRenderData_Right;

extern std::vector<SpriteRenderData> treadmillRenderData_Left;

extern std::vector<SpriteRenderData> decoRenderData;

extern std::vector<SpriteRenderData> ladderRenderData;

extern std::vector<SpriteRenderData> windRenderData;

extern std::vector<SpriteRenderData> waterRenderData;

extern std::vector<SpriteRenderData> trampolineRenderData;

extern std::vector<SpriteRenderData> oneWayRenderData;

extern std::vector<SpriteRenderData> gravityChagerRenderData;

//spikes

extern std::vector<SpriteRenderData> spikeRenderData;

extern std::vector<SpriteRenderData> spikeDobuleRenderData;

extern std::vector<SpriteRenderData> spikeSmallRenderData;

extern std::vector<SpriteRenderData> spikeBallRenderData;

extern std::vector<SpriteRenderData> movingSpikeRenderData;

//platforms

extern std::vector<SpriteRenderData> verticalMovingPlatform_RenderData;

extern std::vector<SpriteRenderData> horizontalMovingPlatform_RenderData;

extern std::vector<SpriteRenderData> fallingPlatform_RenderData;

extern std::vector<SpriteRenderData> disappearingPlatform_RenderData;

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

//UI

extern Texture2D uiBackground;

extern std::vector<SpriteRenderData> portraits;

extern std::vector<SpriteRenderData> uiElements;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern SpriteRenderData LoadRenderData(const char* path, Vector2 frameSize, Vector2 offset = {0,0}, int spacing = 1, int atlasStartFrame = 0, int atlasEndFrame = 0, float animationSpeed = 5.0f);

void LoadAssets();

void UnloadAssets();

inline std::vector<SpriteRenderData>* GetTileActiveRenderDataList(TileType type)
{
    switch (type)
    {
    case TileType::SOLID: return &solidTilesRenderData;

    case TileType::TREADMILL_RIGHT: return &treadmillRenderData_Right;
    case TileType::TREADMILL_LEFT: return &treadmillRenderData_Left;

    case TileType::LADDER: return &ladderRenderData;

    case TileType::ONE_WAY: return &oneWayRenderData;

    case TileType::TRAMPOLINE: return &trampolineRenderData;

    case TileType::GRAVITY_CHANGER: return &gravityChagerRenderData;

    //spikes

    case TileType::SPIKE: return &spikeRenderData;

    case TileType::SPIKE_DOUBLE: return &spikeDobuleRenderData;

    case TileType::SPIKE_SMALL: return &spikeSmallRenderData;

    case TileType::SPIKE_BALL: return &spikeBallRenderData;

    case TileType::VERTICAL_MOVING_SPIKE: return &movingSpikeRenderData;

    case TileType::HORIZONTAL_MOVING_SPIKE: return &movingSpikeRenderData;

    case TileType::ROTATING_SPIKE_SINGLE: return &movingSpikeRenderData;

    case TileType::ROTATING_SPIKE_DOUBLE: return &movingSpikeRenderData;

    //deco

    case TileType::DECO: return &decoRenderData;

    //wind tiles
    
    case TileType::WIND: return &windRenderData;

    //water

    case TileType::WATER: return &waterRenderData;
    
    //platforms

    case TileType::VERTICAL_MOVING_PLATFORM: return &verticalMovingPlatform_RenderData;

    case TileType::HORIZONALT_MOVING_PLATFORM: return &horizontalMovingPlatform_RenderData;

    case TileType::FALLING_PLATFORM: return &fallingPlatform_RenderData;

    case TileType::DISAPPEARING_PLATFORM: return &disappearingPlatform_RenderData;

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
    case PlatformType::MOVING_VERTICAL: return &verticalMovingPlatform_RenderData;
    case PlatformType::MOVING_HORIZONTAL: return &horizontalMovingPlatform_RenderData;

    case PlatformType::FALLING: return &fallingPlatform_RenderData;
    case PlatformType::DISAPPEARING: return &disappearingPlatform_RenderData;

    //moving spikes
    case PlatformType::MOVING_SPIKE_HORIZONTAL: return &movingSpikeRenderData;
    case PlatformType::MOVING_SPIKE_VERTICAL: return &movingSpikeRenderData;
    case PlatformType::ROTATING_SPIKE_SINGLE: return &movingSpikeRenderData;
    case PlatformType::ROTATING_SPIKE_DOUBLE: return &movingSpikeRenderData;

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

inline std::vector<std::array<Color, 4>> LoadPalette(const char* palettePath)
{
    std::vector<std::array<Color, 4>> paletteList = {};

    paletteList.reserve(8);

    const int SQUARE_SIZE = 4; //pixels

    const int GAP = 1; //pixels

    const int STEP = SQUARE_SIZE + GAP;

    Image palleteImg = LoadImage(palettePath);

    Color pixelColor = BLANK;

    for(int y = 0; y < MAX_PALETTE_ROWS; y++)
    {
        std::array<Color, MAX_PALETTE_COLS> currentPalette;

        for(int x = 0; x < MAX_PALETTE_COLS; x++)
        {
            int squareX = x * STEP;
            int squareY = y * STEP;

            int pixelX = squareX + 1;
            int pixelY = squareY + 1;
            
            currentPalette[x] = GetImageColor(palleteImg, pixelX, pixelY);
        }

        paletteList.push_back(currentPalette);
    }

    UnloadImage(palleteImg);

    return paletteList;
}

inline std::vector<std::array<Color, 4>>* GetCurrentTilePaletteList(TileType type)
{
    if(type > TileType::ENEMY_START && type < TileType::ENEMY_END) return &spritePalettes;
    else return &environmentPalettes;

    return nullptr;
}

inline void ChangePalette(int paletteIndex, std::vector<std::array<Color, 4>>* paletteList)
{
    if(paletteIndex >= 0 && paletteIndex < (int)paletteList->size())
    {
        if(paletteIndex != lastPaletteIndex || paletteList != lastPaletteList)
        {   
            rlDrawRenderBatchActive();

            SetShaderPalette(paletteShader, paletteLoc, paletteList->at(paletteIndex));
            
            lastPaletteIndex = paletteIndex;
            lastPaletteList = paletteList;
        }
    }
}