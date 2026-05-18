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

    Tile level[LAYERS][ROWS][COLS];

    std::vector<Platform*> platformList = {};

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
                    Tile& tile = level[l][i][j];

                    tile.textureIndex = 0;
                    tile.type = TileType::VOID;

                    if(tile.gameObj.body)
                    {
                        delete tile.gameObj.body;
                        tile.gameObj.body = nullptr;
                    }
                }
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

            float step = 1.0f / (float)GRID_SIZE;

            camera.zoom = roundf(camera.zoom / step) * step;

            camera.zoom = Clamp(camera.zoom, 0.25f, 15.25f);
        }
    }

    inline bool IsOneWayUpDown(Tile tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.gameObj.direction == Direction::UP || tile.gameObj.direction == Direction::DOWN)
                return true;
        }

        return false;
    }

    inline bool IsOneWayRightLeft(Tile tile)
    {
        if(tile.type == TileType::ONE_WAY)
        {
            if(tile.gameObj.direction == Direction::RIGHT || tile.gameObj.direction == Direction::LEFT)
                return true;
        }

        return false;
    }

    inline bool IsTileOneWay(Tile tile)
    {
        return IsOneWayRightLeft(tile) || IsOneWayUpDown(tile);
    }

    void DiscreteUpdate();

    void DebugDrawing();

    void DebugTextDrawing();

    inline bool IsNotRealTile(TileType type)
    {
        if(type == TileType::VOID) return true;

        if(IsTypeInvalid(type)) return true;

        if(type == TileType::PLAYER_SPAWN) return true;

        if(type >= TileType::PLATFORM_START && type <= TileType::PLATFORM_END) return true;

        return false;
    }

    inline bool IsPlatformFarFromPlayer(Vector2 platformPosition, float maxDistance = MAX_DISTANCE_PLATFORM_PLAYER_SQR)
    {
        return Vector2LengthSqr(Vector2Subtract(platformPosition, player.phys.transform.position)) > maxDistance;
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