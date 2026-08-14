#pragma once

#include <vector>

#include "raylib.h"
#include "player.h"
#include "physics.h"
#include "drawing.h"
#include "platform.h"
#include "leveldata.h"
#include "enemy.h"

const float MAX_DISTANCE_PLATFORM_PLAYER_SQR = 500 * 500;

const float MAX_DISTANCE_PLATFORM_PLAYER_SQR_5X = MAX_DISTANCE_PLATFORM_PLAYER_SQR * 5;

const float REC_TO_CIRCLE_RADIUS_MULTIPLIER = 1.5f; //this ensures that the circle doesn't cut the corners of the AABB

const float CAMERA_ZOOM = 1.0f;

const float ENEMY_SPAWN_RADIUS = TILE_SIZE * TILES_PER_ROOM_WIDHT;

const float ENEMY_DESPAWN_RADIUS = ENEMY_SPAWN_RADIUS + TILE_SIZE;

const float PLATFORM_UPDATE_RADIUS = TILE_SIZE * 28.0f;

struct TileRangeLimits
{
    int minX = 0;
    int minY = 0;
    int maxX = ROWS - 1;
    int maxY = COLS - 1;
};

struct Level
{
    //level
    Tile level[LAYERS][COLS][ROWS];

    std::vector<Enemy> enemyList = {};

    std::vector<Platform> platformList = {};

    std::vector<Room> rooms = {};

    std::vector<std::vector<Enemy>> enemyBuckets = {};
    std::vector<std::vector<Platform>> platformBuckets = {};

    std::vector<Platform*> platformCache_update = {};
    std::vector<Platform*> platformCache_physics = {};
    std::vector<Platform*> platformCache_rendering = {};

    std::vector<Enemy*> enemyCache = {};
    std::vector<Enemy*> enemyCache_physics = {};

    Player player = {};

    RenderTexture2D gameplayCanvas = {};

    RenderTexture2D uiCanvas = {};

    Camera2D camera = {};
    
    //clamps the tile range to the current room
    TileRangeLimits rangeLimits = {};

    Vector2 screenShakeOffset = {0,0};

    //screen shake
    float screenShakeTimer = 0.0f;

    float screenShakeDuration = 0.0f;

    float dt;

    float gravity = 500.0f;

    int iterations = 1;

    int lowFrequencyCounter = 0;

    int collisionTileCheckRange = 2;

    int renderTileCheckRange = 15;

    int previousRoomIndex = -1;

    int currentRoomIndex = -1;

    int screenShakeMagnitude = 0; //screen shake

    bool isGravityUp = false;

    bool debugDrawing = false;

    bool paused = false;

    Level() = default;

    ~Level();

    void InitLevel(const char* levelPath, const char* roomPath ,float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();

    void DrawLevelUI();

    void ResetLevel();

    void ResetRoom();

    void Pause();

    inline void ClearTileMatrix()
    {
        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = 0; i < COLS; i++)
            {
                for(int j = 0; j < ROWS; j++)
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

    //camera

    void UpdateCamera(const Vector2& target, const Vector2& offset);

    inline void TriggerScreenShake(float duration, int magnitude)
    {
        screenShakeTimer = 0.0f;
        screenShakeDuration = duration;
        screenShakeMagnitude = magnitude;
    }

    inline void CalculateScreenShake()
    {
        if(screenShakeTimer < screenShakeDuration)
        {
            screenShakeTimer += dt;

            float timePercentage = screenShakeTimer / screenShakeDuration;

            timePercentage = Clamp(timePercentage, 0.0f, 1.0f);

            int maxOffset = (int)Lerp(screenShakeMagnitude, 0.0f, timePercentage);

            if(maxOffset > 0)
            {
                screenShakeOffset = {
                    (float)GetRandomValue(-maxOffset, maxOffset),
                    (float)GetRandomValue(-maxOffset, maxOffset)
                };
            }
        }
        else
        {
            screenShakeOffset = {0,0};
        }
    }

    //camera end

    inline bool IsOneWayUpDown(const Tile& tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.direction == Direction::UP || tile.direction == Direction::DOWN)
                return true;
        }

        return false;
    }

    inline bool IsOneWayRightLeft(const Tile& tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.direction == Direction::RIGHT || tile.direction == Direction::LEFT)
                return true;
        }

        return false;
    }

    inline bool IsTileOneWay(const Tile& tile)
    {
        return IsOneWayRightLeft(tile) || IsOneWayUpDown(tile);
    }

    inline Vector2 SetUIElementPosition(int x, int y)
    {
        Vector2 pos = {(float)TILE_SIZE * x, (float)TILE_SIZE * y};

        return pos;
    }

    inline Vector2 SetUIElementPositionCentered(int x, int y)
    {
        float halfTileSize = TILE_SIZE * 0.5f;

        Vector2 pos = {TILE_SIZE * x + halfTileSize, TILE_SIZE * y + halfTileSize};

        return pos;
    }

    void LowFrequencyUpdate(); //less than 60 fps

    void MediumFrequencyDiscreteUpdate_First(); //60 fps

    void MediumFrequencyDiscreteUpdate_Second(); //60 fps

    void HighFrequencyDiscreteUpdate(); //60 fps at 10 iterations (600 times total)

    void BulletsUpdate();

    void PauseDrawing();

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

    inline TileRangeLimits GetTileRangeLimits()
    {
        TileRangeLimits rangeLimits = {};

        if(currentRoomIndex > -1)
        {
            Rectangle& currentRoom = rooms[currentRoomIndex].aabb;

            rangeLimits = {
                (int)(currentRoom.x / TILE_SIZE),
                (int)(currentRoom.y / TILE_SIZE),
                (int)((currentRoom.x + currentRoom.width) / TILE_SIZE) - 1,
                (int)((currentRoom.y + currentRoom.height) / TILE_SIZE) - 1
            };
        }

        return rangeLimits;
    }

    inline void UpdatePlayerInput()
    {
        player.UpdateInput();

        if(IsKeyPressed(KEY_D)) debugDrawing =  true;
        else if(IsKeyPressed(KEY_F)) debugDrawing = false;
    }

    inline void EvaluateLevelPause()
    {
        paused = player.pausePressed;
    }

    inline int GetPlatformCount()
    {
        return (int)platformList.size();
    }

    inline int GetPlatformCache_Update()
    {
        return (int)platformCache_update.size();
    }

    inline int GetPlatformCache_Physics()
    {
        return (int)platformCache_physics.size();
    }

    inline int GetPlatformCache_Render()
    {
        return (int)platformCache_rendering.size();
    }

    inline int GetEnemyCount()
    {
        return (int)enemyList.size();
    }

    inline int GetEnemyCache()
    {
        return (int)enemyCache.size();
    }

    inline int GetEnemyCache_Physics()
    {
        return (int)enemyCache_physics.size();
    }
};