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

const float CAMERA_ZOOM = 1.2f;

const float ENEMY_SPAWN_RADIUS = GRID_SIZE * 15.0f;

const float ENEMY_DESPAWN_RADIUS = GRID_SIZE * 17.0f;

const float PLATFORM_UPDATE_RADIUS = GRID_SIZE * 25.0f;

struct TileRangeLimits
{
    int minX = 0;
    int minY = 0;
    int maxX = ROWS - 1;
    int maxY = COLS - 1;
};

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

    //screen shake

    float screenShakeTimer = 0.0f;
    float screenShakeDuration = 0.0f;
    int screenShakeMagnitude = 0;

    Vector2 screenShakeOffset = {0,0};

    //level
    Tile level[LAYERS][COLS][ROWS];

    std::vector<Room> rooms = {};

    std::vector<Platform> platformList = {};
    std::vector<Enemy> enemyList = {};

    std::vector<std::vector<Enemy>> enemyBuckets = {};
    std::vector<std::vector<Platform>> platformBuckets = {};

    std::vector<Platform*> platformCache_update = {};
    std::vector<Platform*> platformCache_physics = {};
    std::vector<Platform*> platformCache_rendering = {};

    std::vector<Enemy*> enemyCache = {};
    std::vector<Enemy*> enemyCache_physics = {};

    int previousRoomIndex = -1;
    int currentRoomIndex = -1;

    Camera2D camera = {};

    Player player;

    //clamps the tile range to the current room
    TileRangeLimits rangeLimits = {};

    void ResetRoom();

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

    //change the target relative to the speed of the player
    //so the camera points a bit ahead of where the player is moving

    //camera
    inline void UpdateCamera(const Vector2& target, const Vector2& offset)
    {
        int roomIndex = -1;

        Rectangle currentRoom = {};

        for(int r = 0; r < rooms.size(); r++)
        {
            if(CheckCollisionPointRec(target, rooms[r].aabb))
            {
                roomIndex = r;
                currentRoom = rooms[r].aabb;

                break;
            }
        }

        Vector2 desired = Vector2Add(target, offset);

        if(roomIndex > -1)
        {
            Vector2 halfScreenWorld = {
                (screenWidth * 0.5f) / camera.zoom,
                (screenHeight * 0.5f) / camera.zoom
            };

            Vector2 min = {
                currentRoom.x + halfScreenWorld.x,
                currentRoom.y + halfScreenWorld.y
            };

            Vector2 max = {
                (currentRoom.x + currentRoom.width) - halfScreenWorld.x,
                (currentRoom.y + currentRoom.height) - halfScreenWorld.y
            };

            if(currentRoom.width < (screenWidth / camera.zoom))
            {
                desired.x = currentRoom.x + (currentRoom.width * 0.5f);
            }
            else
            {
                desired.x = Clamp(desired.x, min.x, max.x);
            }

            if(currentRoom.height < (screenHeight/ camera.zoom))
            {
                desired.y = currentRoom.y + (currentRoom.height * 0.5f);
            }
            else
            {
                desired.y = Clamp(desired.y, min.y, max.y);
            }
            
        }

        //commented out
        //camera.target = Vector2Lerp(camera.target, desired, 0.1f);

        camera.target.x = desired.x;
        camera.target.y = desired.y;

        float zoom = camera.zoom;

        camera.target.x = floorf(camera.target.x * zoom) / zoom;
        camera.target.y = floorf(camera.target.y * zoom) / zoom;

        camera.offset = {floorf(screenWidth * 0.5f), floorf(screenHeight * 0.5f)};

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

    inline TileRangeLimits GetTileRangeLimits()
    {
        TileRangeLimits rangeLimits = {};

        if(currentRoomIndex > -1)
        {
            Rectangle& currentRoom = rooms[currentRoomIndex].aabb;

            rangeLimits = {
                (int)(currentRoom.x / GRID_SIZE),
                (int)(currentRoom.y / GRID_SIZE),
                (int)((currentRoom.x + currentRoom.width) / GRID_SIZE) - 1,
                (int)((currentRoom.y + currentRoom.height) / GRID_SIZE) - 1
            };
        }

        return rangeLimits;
    }

public:

    int screenWidth, screenHeight;

    RenderTexture2D canvas = {};

    Level();

    ~Level();

    void InitLevel(const char* levelPath, const char* roomPath ,float dt, int iterations);

    void UpdateLevel();

    void DrawLevel();

    void ResetLevel();

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