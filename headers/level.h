#pragma once

#include <vector>

#include "raylib.h"
#include "player.h"
#include "physics.h"
#include "drawing.h"
#include "platform.h"
#include "leveldata.h"
#include "enemy.h"

const float MAX_DISTANCE_PLATFORM_PLAYER_SQR = 800 * 800;

const float MAX_DISTANCE_PLATFORM_PLAYER_SQR_5X = MAX_DISTANCE_PLATFORM_PLAYER_SQR * 5;

const float REC_TO_CIRCLE_RADIUS_MULTIPLIER = 1.5f; //this ensures that the circle doesn't cut the corners of the AABB

const float GRAVITY = 3500.0f;

class Level
{
private:

    float dt;

    int iterations = 1;

    int lowFrequencyCounter = 0;

    int collisionTileCheckRange = 2;

    int renderTileCheckRange = 15;

    float gravity = 500;

    bool isGravityUp = false;

    Tile level[LAYERS][ROWS][COLS];

    std::vector<Platform> platformList = {};

    std::vector<Enemy> enemyList = {};

    Camera2D camera = {};

    Player player;

    inline void ClearTileMatrix()
    {
        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = 0; i < ROWS; i++)
            {
                for(int j = 0; j < COLS; j++)
                {
                    level[l][i][j] = {};
                }
            }
        }
    }

    inline void ClearPlatformList()
    {
        platformList.clear();
    }

    inline void UpdateCamera(
       const Vector2& target, const Vector2& offset
    )
    {
        //change the target relative to the speed of the player
        //so the camera points a bit ahead of where the player is moving
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

            float step = 1.0f / (float)GRID_SIZE;

            camera.zoom = roundf(camera.zoom / step) * step;

            camera.zoom = Clamp(camera.zoom, 0.25f, 15.25f);
        }
    }

    inline bool IsOneWayUpDown(const Tile& tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.gameObj.direction == Direction::UP || tile.gameObj.direction == Direction::DOWN)
                return true;
        }

        return false;
    }

    inline bool IsOneWayRightLeft(const Tile& tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.gameObj.direction == Direction::RIGHT || tile.gameObj.direction == Direction::LEFT)
                return true;
        }

        return false;
    }

    inline bool IsTileOneWay(const Tile& tile)
    {
        return IsOneWayRightLeft(tile) || IsOneWayUpDown(tile);
    }

    void LowFrequencyUpdate(); //less than 60 fps

    void MediumFrequencyDiscreteUpdate(); //60 fps

    void HighFrequencyDiscreteUpdate(); //60 fps at 10 iterations (600 times total)

    void CCD_Update();

    void DebugDrawing();

    void DebugTextDrawing();

    inline bool IsNotRealTile(const TileType& type)
    {
        if(type == TileType::VOID) return true;

        if(IsTypeInvalid(type)) return true;

        if(type == TileType::PLAYER_SPAWN) return true;

        if(type >= TileType::PLATFORM_START && type <= TileType::PLATFORM_END) return true;

        if(type >= TileType::ENEMY_START && type <= TileType::ENEMY_END) return true;

        return false;
    }

    inline bool IsPlatformFarFromPlayer(const Vector2& platformPosition, float maxDistance = MAX_DISTANCE_PLATFORM_PLAYER_SQR)
    {
        return Vector2LengthSqr(Vector2Subtract(platformPosition, player.gameObj.transform.position)) > maxDistance;
    }

    inline bool IsTileSpike(const TileType& type)
    {
        return (type > TileType::SPIKE_START && type < TileType::SPIKE_END);
    }

    inline bool CanEnemyCollideWithTile(const TileType& type)
    {
        switch (type)
        {
        case TileType::SOLID:
        case TileType::TRAMPOLINE:
        case TileType::TREADMILL_LEFT:
        case TileType::TREADMILL_RIGHT:
        case TileType::ONE_WAY:
        case TileType::WATER:
            return true;
        default: return false;
        }
    }

public:

    int screenWidth, screenHeight;

    Level();

    ~Level();

    void InitLevel(const char* levelPath, float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();

    void ResetLevel();

    inline int GetPlatformCount()
    {
        return (int)platformList.size();
    }

    inline int GetPlayerPlatformCache_Update()
    {
        return (int)player.platformCache_update.size();
    }

    inline int GetPlayerPlatformCache_Physics()
    {
        return (int)player.platformCache_physics.size();
    }

    inline int GetPlayerPlatformCache_Render()
    {
        return (int)player.platformCache_rendering.size();
    }

    inline int GetEnemyCount()
    {
        return (int)enemyList.size();
    }

    inline int GetPlayerEnemyCache()
    {
        return (int)player.enemyCache.size();
    }

    inline int GetPlayerEnemyCache_Physics()
    {
        return (int)player.enemyCache_physics.size();
    }
};