#pragma once

#include <vector>

#include "raylib.h"
#include "player.h"
#include "physics.h"
#include "drawing.h"
#include "platform.h"
#include "leveldata.h"


const float MAX_DISTANCE_PLATFORM_PLAYER = 700;

class Level
{
private:

    float dt;

    int iterations = 1;

    int tileCheckRange = 2;

    float gravity = 500;

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

    inline void UpdateCamera(
        Vector2 target, Vector2 offset
    )
    {
        Vector2 desired = Vector2Add(target, offset);

        camera.target = Vector2Lerp(camera.target, desired, 0.1f);

        camera.offset = {screenWidth * 0.5f, screenHeight * 0.5f};

        camera.rotation = 0.0f;

        int mouseWheel = GetMouseWheelMove();

        float cameraFactor = 0.25f;

        if(IsKeyDown(KEY_LEFT_ALT))
        {
            if(mouseWheel > 0) camera.zoom += cameraFactor;
            else if(mouseWheel < 0) camera.zoom -= cameraFactor;

            camera.zoom = Clamp(camera.zoom, 0.25f, 15.25f);
        }
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

    inline bool IsTileJumpTrigger(int i, int j)
    {
        const TileType& type = level[i][j].type;

        return type == TileType::SPIKE || 
        type == TileType::TRAMPOLINE ||
        type == TileType::GRAVITY_CHANGER ||
        type == TileType::GOAL ||
        type == TileType::ONE_WAY_RIGHT ||
        type == TileType::ONE_WAY_LEFT ||
        type == TileType::PLATFORM_STOP;
    }

    void DiscreteUpdate();

    void DebugDrawing();

    void DebugTextDrawing();

    void LoadLevelData(const char* levelPath);

    inline bool IsNotRealTile(int i, int j)
    {
        TileType type = level[i][j].type;

        return  type == TileType::VOID || 
                type == TileType::PLAYER_SPAWN ||
                type == TileType::HORIZONALT_MOVING_PLATFORM ||
                type == TileType::VERTICAL_MOVING_PLATFORM ||
                type == TileType::FALLING_PLATFORM ||
                type == TileType::DISAPPEARING_PLATFORM ||
                type == TileType::COUNT;
    }

public:

    int screenWidth, screenHeight;

    Level();

    ~Level();

    void InitLevel(const char* levelPath, float gridSize, float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();
};