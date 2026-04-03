#pragma once 

#include "raylib.h"
#include "entity.h"


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

static int gridSize = 50;

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
    SOLID,
    GOAL,
    PLATFORM_STOP,
    TRAMPOLINE,
    GRAVITY_CHANGER,
    TREADMILL_RIGHT,
    TREADMILL_LEFT,
    ONE_WAY_UP,
    ONE_WAY_DOWN,
    ONE_WAY_RIGHT,
    ONE_WAY_LEFT,
    SPIKE,
    HORIZONALT_MOVING_PLATFORM,
    VERTICAL_MOVING_PLATFORM,
    FALLING_PLATFORM,
    DISAPPEARING_PLATFORM,
    PLAYER_SPAWN,
    COUNT
};

struct Tile
{
    TileType type = TileType::VOID;
    SpriteRenderData renderData = {};
};

inline bool IsColorOf(Color colorA, Color colorB)
{
    return colorA.r == colorB.r && 
    colorA.g == colorB.g && 
    colorA.b == colorB.b &&
    colorA.a == colorB.a;
}

inline bool IsTileEmpty(int i, int j, Tile(&levelTiles)[ROWS][COLS])
{
    if(i < 0 || i >= ROWS || j < 0 || j >= COLS) return true;

    return levelTiles[i][j].type == TileType::VOID;
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
    switch (type)
    {
    case TileType::VOID: return BLANK;
    case TileType::SOLID: return BLACK;
    case TileType::GOAL: return GOAL;
    case TileType::PLATFORM_STOP: return PLATFORM_STOP;
    case TileType::TRAMPOLINE: return TRAMPOLINE;
    case TileType::TREADMILL_RIGHT: return TREADMILL_RIGHT;
    case TileType::TREADMILL_LEFT: return TREADMILL_LEFT;
    case TileType::ONE_WAY_UP: return ONE_WAY_UP;
    case TileType::ONE_WAY_DOWN: return ONE_WAY_DOWN;
    case TileType::ONE_WAY_RIGHT: return ONE_WAY_RIGHT;
    case TileType::ONE_WAY_LEFT: return ONE_WAY_LEFT;
    case TileType::GRAVITY_CHANGER: return GRAVITY_CHANGER;
    case TileType::SPIKE: return SPIKE;
    case TileType::HORIZONALT_MOVING_PLATFORM: return HORIZONTAL_MOVING_PLATFORM;
    case TileType::VERTICAL_MOVING_PLATFORM: return VERTICAL_MOVING_PLATFORM;
    case TileType::FALLING_PLATFORM: return FALLING_PLATFORM;
    case TileType::DISAPPEARING_PLATFORM: return DISAPPEARING_PLATFORM;
    case TileType::PLAYER_SPAWN: return PLAYER_SPAWN;
    
    default: return BLANK;
    }
}

inline const char* GetTileTypeText(TileType type)
{
    switch (type)
    {
    case TileType::VOID: return "VOID";
    case TileType::SOLID: return "SOLID";
    case TileType::GOAL: return "GOAL";
    case TileType::PLATFORM_STOP: return "PLATFORM_STOP";
    case TileType::TRAMPOLINE: return "TRAMPOLINE";
    case TileType::TREADMILL_RIGHT: return "TREADMILL_RIGHT";
    case TileType::TREADMILL_LEFT: return "TREADMILL_LEFT";
    case TileType::ONE_WAY_UP: return "ONE_WAY_UP";
    case TileType::ONE_WAY_DOWN: return "ONE_WAY_DOWN";
    case TileType::ONE_WAY_RIGHT: return "ONE_WAY_RIGHT";
    case TileType::ONE_WAY_LEFT: return "ONE_WAY_LEFT";
    case TileType::GRAVITY_CHANGER: return "GRAVITY_CHANGER";
    case TileType::SPIKE: return "SPIKE";
    case TileType::HORIZONALT_MOVING_PLATFORM: return "HORIZONTAL_MOVING_PLATFORM";
    case TileType::VERTICAL_MOVING_PLATFORM: return "VERTICAL_MOVING_PLATFORM";
    case TileType::FALLING_PLATFORM: return "FALLING_PLATFORM";
    case TileType::DISAPPEARING_PLATFORM: return "DISAPPEARING_PLATFORM";
    case TileType::PLAYER_SPAWN: return "PLAYER_SPAWN";

    default: return "BLANK";
    }
}