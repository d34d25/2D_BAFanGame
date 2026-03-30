#pragma once

#include <vector>

#include "raylib.h"
#include "player.h"
#include "physics.h"
#include "drawing.h"
#include "platform.h"

static constexpr Color PLAYER_SPAWN = Color{251,242,54,255};

static constexpr Color GOAL = Color{0,255,0,255};

static constexpr Color HORIZONTAL_MOVING_PLATFORM = Color{0,100,255,255};

static constexpr Color VERTICAL_MOVING_PLATFORM = Color{255,100,0,255};

static constexpr Color MOVING_PLATFORM_STOP = Color{255,100,255,255};

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

const int ROWS = 70;
const int COLS = 70;

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
    SPIKE
};

struct Tile
{
    TileType type = TileType::VOID;
    SpriteRenderData renderData = {};

    bool isSolid;
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

const float MAX_DISTANCE_PLATFORM_PLAYER = 700;

class Level
{
private:

    float dt;

    int iterations = 1;

    int tileCheckRange = 2;

    float gridSize = 50;

    float gravity = 500;

    Image levelImage;

    Tile level[ROWS][COLS];

    GameObject* gameObjTiles[ROWS][COLS] = {nullptr};

    std::vector<Platform*> platformList = {};

    Camera2D camera = {};

    Player player;

    inline void ClearTileMatrix()
    {
        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLS; j++)
            {
                Tile& tile = level[i][j];

                tile.isSolid = false;
                tile.renderData = {};
                tile.type = TileType::VOID;
            }
        }
    }

    inline void ClearGameObjMatrix()
    {
        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLS; j++)
            {
                if(gameObjTiles[i][j])
                    delete gameObjTiles[i][j];
                
                gameObjTiles[i][j] = nullptr;
            }
        }
    }

    inline void ClearPlatformList()
    {
        for(int i = 0; i < platformList.size(); i++)
        {
            delete platformList[i];
        }

        platformList.clear();
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

    inline void UpdateCamera(
        Vector2 target, Vector2 offset
    )
    {
        Vector2 desired = Vector2Add(target, offset);

        camera.target = Vector2Lerp(camera.target, desired, 0.1f);

        camera.offset = {screenWidth * 0.5f, screenHeight * 0.5f};

        camera.rotation = 0.0f;
        camera.zoom = 1.25f;
    }

    inline bool IsOneWayUpDown(int i, int j)
    {
        const Tile& tile = level[i][j];

        return tile.type == TileType::ONE_WAY_UP || tile.type == TileType::ONE_WAY_DOWN;  
    }

    inline bool IsOneWayRightLeft(int i, int j)
    {
        const Tile& tile = level[i][j];

        return tile.type == TileType::ONE_WAY_RIGHT || tile.type == TileType::ONE_WAY_LEFT;
    }

    inline bool IsOneWayTile(int i, int j)
    {
        return IsOneWayRightLeft(i, j) || IsOneWayUpDown(i, j);
    }

    void DiscreteUpdate();

    void DebugDrawing();

    void DebugTextDrawing();

public:

    int screenWidth, screenHeight;

    Level();

    ~Level();

    void InitLevel(const char* levelPath, float gridSize, float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();
};