#pragma once

#include <vector>

#include "raylib.h"
#include "player.h"
#include "physics.h"
#include "drawing.h"
#include "platform.h"
#include "leveldata.h"


const float MAX_DISTANCE_PLATFORM_PLAYER_SQR = 800 * 800;

const float GRAVITY = 3500.0f;

class Level
{
private:

    float dt;

    int iterations = 1;

    int collisionTileCheckRange = 2;

    int renderTileCheckRange = 15;

    float gravity = 500;

    Tile level[ROWS][COLS];

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

                tile.textureIndex = 0;
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
                if(level[i][j].gameObj)
                    delete level[i][j].gameObj;
                
                level[i][j].gameObj = nullptr;
            }
        }
    }

    inline void ClearPlatformList()
    {
        for(int i = 0; i < platformList.size(); i++)
        {
            if(platformList[i])
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

        float zoom = camera.zoom;

        camera.target.x = floorf(camera.target.x * zoom) / zoom;
        camera.target.y = floorf(camera.target.y * zoom) / zoom;

        camera.offset = {floorf(screenWidth * 0.5f), floorf(screenHeight * 0.5f)};

        camera.rotation = 0.0f;

        int mouseWheel = GetMouseWheelMove();

        float cameraFactor = 0.2f;

        if(IsKeyDown(KEY_LEFT_ALT))
        {
            if(mouseWheel > 0) camera.zoom += cameraFactor;
            else if(mouseWheel < 0) camera.zoom -= cameraFactor;

            float step = 1.0f / (float)gridSize;

            camera.zoom = roundf(camera.zoom / step) * step;

            camera.zoom = Clamp(camera.zoom, 0.25f, 15.25f);
        }
    }

    inline bool IsOneWayUpDown(TileType type)
    {
        return type == TileType::ONE_WAY_UP || type == TileType::ONE_WAY_DOWN;  
    }

    inline bool IsOneWayRightLeft(TileType type)
    {
        return type == TileType::ONE_WAY_RIGHT || type == TileType::ONE_WAY_LEFT;
    }

    inline bool IsTileOneWay(TileType type)
    {
        return IsOneWayRightLeft(type) || IsOneWayUpDown(type);
    }

    inline bool IsTileWind(TileType type)
    {
        return type == TileType::WIND_UP ||
        type == TileType::WIND_DOWN ||
        type == TileType::WIND_LEFT ||
        type == TileType::WIND_RIGHT;
    }

    inline bool IsTileNotJumpTrigger(TileType type)
    {
        return type == TileType::TRAMPOLINE ||
        type == TileType::GRAVITY_CHANGER ||
        type == TileType::GOAL ||
        type == TileType::ONE_WAY_RIGHT ||
        type == TileType::ONE_WAY_LEFT ||
        type == TileType::PLATFORM_STOP ||
        (type > TileType::SPIKE_START && type < TileType::SPIKE_END) ||
        IsTileWind(type) ||
        type == TileType::WATER;
    }

    void DiscreteUpdate();

    void DebugDrawing();

    void DebugTextDrawing();

    void LoadLevelData(const char* levelPath);

    inline bool IsNotRealTile(TileType type)
    {
        if(type == TileType::VOID) return true;

        if(IsTypeInvalid(type)) return true;

        if(type >=  TileType::LOGIC_START && type <= TileType::LOGIC_END) return true;

        if(type >= TileType::PLATFORM_START && type <= TileType::PLATFORM_END) return true;

        return false;
    }

    inline bool IsPlatformFarFromPlayer(Vector2 platformPosition)
    {
        return Vector2LengthSqr(Vector2Subtract(platformPosition, player.phys.position)) > MAX_DISTANCE_PLATFORM_PLAYER_SQR;
    }

    inline bool IsTileSpike(TileType type)
    {
        return (type > TileType::SPIKE_START && type < TileType::SPIKE_END);
    }

public:

    int screenWidth, screenHeight;

    Level();

    ~Level();

    void InitLevel(const char* levelPath, float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();
};