#include "level.h"

#include <iostream>

Level::Level() : player({0, 0})
{
}

Level::~Level()
{
    ClearTileMatrix();

    ClearGameObjMatrix();

    ClearPlatformList();
}

void Level::LoadLevelData(const char *levelPath)
{
    int dataSize = 0;

    unsigned char* fileData = LoadFileData(levelPath, &dataSize);

    if(fileData == nullptr) return;

    TileType* loadedTypes = (TileType*)fileData;

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            TileType type = loadedTypes[i * COLS + j];

            level[i][j].type = type;
        }
    }

    UnloadFileData(fileData);
}

void Level::InitLevel(const char *levelPath, float gridSize, float dt, int iterations)
{
    this->iterations = iterations;

    this->dt = dt;

    gravity = 3500;

    player.entityData.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    ClearTileMatrix();

    ClearGameObjMatrix();

    ClearPlatformList();

    LoadLevelData(levelPath);

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            if(level[i][j].type == TileType::COUNT) level[i][j].type = TileType::VOID;

            TileType type = level[i][j].type;
            
            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

            if(type == TileType::PLAYER_SPAWN) player.phys.position = {xpos, ypos};
           
            //platforms...

            bool isPlatform = type == TileType::HORIZONALT_MOVING_PLATFORM || type == TileType::VERTICAL_MOVING_PLATFORM
            || type == TileType::FALLING_PLATFORM || type == TileType::DISAPPEARING_PLATFORM;

            if(isPlatform)
            {
                Platform* platform = new Platform();

                float platformWidth = gridSize * 3.0f;
                float platformHeight = gridSize * 0.3f;

                platform->gravity = gravity;

                if(type == TileType::FALLING_PLATFORM || type == TileType::DISAPPEARING_PLATFORM)
                {
                    platformWidth = gridSize;
                    platformHeight = gridSize;
                }

                platform->phys.position = {xpos, ypos};

                platform->phys.aabb.width = platformWidth;
                platform->phys.aabb.height = platformHeight;

                platform->phys.UpdateAABB();

                float platformSpeed = 100.0f;

                platform->SetTimer(0.3f);

                if(type == TileType::HORIZONALT_MOVING_PLATFORM)
                {
                    platform->isHorizontal = true;

                    platform->phys.body.velocity.x = platformSpeed;

                    platform->updateRequired = true;
                }
                else if(type == TileType::VERTICAL_MOVING_PLATFORM)
                {
                    platform->isVertical = true;

                    platform->phys.body.velocity.y = platformSpeed;

                    platform->updateRequired = true;
                }
                else if(type == TileType::FALLING_PLATFORM)
                {
                    platform->isFalling = true;
                    platform->phys.body.hasGravity = true;
                }
                else if(type == TileType::DISAPPEARING_PLATFORM)
                {
                    platform->isDisappearing = true;
                }

                platformList.push_back(platform);
            }
        }
    }

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            Tile* tile = &level[i][j];
            TileType type = tile->type;

            if(IsNotRealTile(i,j)) continue;

            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

            gameObjTiles[i][j] = new GameObject();

            GameObject* objTile = gameObjTiles[i][j];

            objTile->aabb = {0, 0, gridSize, gridSize};

            objTile->position = {xpos, ypos};

            objTile->UpdateAABB();

            float treadmillVel = 100.0f;

            switch (type)
            {
            case TileType::SOLID:
            {
                tile->isSolid = true; //change later

                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::GOAL:
            {
                objTile->canEntityCollide = false;
                objTile->canPlatformCollide = false;
            }
            break;

            case TileType::PLATFORM_STOP:
            {
                objTile->canEntityCollide = false;
                objTile->canPlatformCollide = true;
            }
            break;
            
            case TileType::TRAMPOLINE:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::GRAVITY_CHANGER:
            {
                objTile->canEntityCollide = false;
                objTile->canPlatformCollide = false;
            }
            break;

            case TileType::TREADMILL_RIGHT:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;

                objTile->body.velocity.x = treadmillVel;
            }
            break;

            case TileType::TREADMILL_LEFT:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;

                objTile->body.velocity.x = -treadmillVel;
            }
            break;

            case TileType::ONE_WAY_UP:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::ONE_WAY_DOWN:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::ONE_WAY_RIGHT:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::ONE_WAY_LEFT:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            case TileType::SPIKE:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;
            }
            break;

            default:
            {
                objTile->canEntityCollide = false;
                objTile->canPlatformCollide = false;
            }
            break;
            }
        }
    }
}

void Level::UpdateLevel()
{
    for(int iteraion = 0; iteraion < iterations; iteraion++)
    {
        DiscreteUpdate();
    }

    player.UpdateRender();

    UpdateCamera(player.phys.position, {0, -100});
}

void Level::DiscreteUpdate()
{
    bool isGravityUp = gravity < 0;

    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        tileCheckRange
    );

    player.Update(dt, iterations);

    player.phys.UpdatePositionX(dt, iterations);

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = gameObjTiles[i][j];            

            if(!objTile) continue;

            if(!objTile->canEntityCollide) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                SolveCollisions(&player.phys, objTile, true, isGravityUp);
            }
            else if(IsOneWayRightLeft(i, j))
            {
                SolveCollisionsOneWayLeftRight(
                    &player.phys, objTile,
                    tile.type == TileType::ONE_WAY_RIGHT
                );
            }
        }
    }


    player.phys.UpdatePositionY(dt, iterations);

    bool isPlayerGrounded = false;

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = gameObjTiles[i][j];

            if(!objTile) continue;            

            if(!objTile->canEntityCollide) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                SolveCollisions(&player.phys, objTile, false, isGravityUp);
            }
            else if(IsOneWayUpDown(i, j))
            {
                SolveCollisionsOneWayUpDown
                (
                    &player.phys, objTile,
                    tile.type == TileType::ONE_WAY_UP,
                    isGravityUp
                );
            }

            if(IsOneWayRightLeft(i,j)) continue;

            //check for direction in the case of one way tiles
            if(CheckCollisionRecs(player.GetJumpDetector(), objTile->aabb))
                isPlayerGrounded = true;
        }
    }

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(platform->IsInactive()) continue;

        if(platform->updateRequired) platform->Update(dt, iterations);

        SolveCollisionsOneWayUpDown(
            &player.phys, &platform->phys,
            true, isGravityUp
        );

        if(CheckCollisionRecs(player.GetJumpDetector(), platform->phys.aabb))
        {
            platform->updateRequired = true;

            isPlayerGrounded = true;
        }
    }

    
    player.canJump = isPlayerGrounded;
}

void Level::DrawLevel()
{
    BeginMode2D(camera);

    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        14
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Color platformColor = HORIZONTAL_MOVING_PLATFORM;

        Platform* platform = platformList[i];

        if(!platform) continue;

        if(platform->isVertical) platformColor = VERTICAL_MOVING_PLATFORM;
        else if(platform->isFalling) platformColor = FALLING_PLATFORM;
        else if(platform->isDisappearing) platformColor = DISAPPEARING_PLATFORM;

        DrawRectangleRec(platform->phys.aabb, platformColor);
    }

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            TileType type = level[i][j].type;

            if(IsNotRealTile(i,j)) continue;

            Color color = GetTileColor(type);

            if(IsColorOf(color, BLANK)) continue;

            if(gameObjTiles[i][j]) DrawRectangleRec(gameObjTiles[i][j]->aabb, color);
        }
    }

    DrawSprite(
        player.characterRenderData,
        player.entityData
    );

    DrawSprite(
        player.weaponRenderData,
        player.entityData
    );

    DebugDrawing();

    EndMode2D();

    DebugTextDrawing();
}

void Level::DebugDrawing()
{
    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        14
    );

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            if(!gameObjTiles[i][j]) continue;

            DrawAABB(gameObjTiles[i][j]->aabb, RED);
        }
    }

    DrawAABB(player.phys.aabb, ORANGE);

    DrawAABB(player.GetJumpDetector(), GREEN);
}

void Level::DebugTextDrawing()
{
    DrawText(TextFormat("Iterations: %i", iterations), 10, 60, 20, SKYBLUE);

    DrawText(TextFormat("Player X speed: %.4f", player.phys.body.velocity.x), 10, 100, 20, GRAY);
    DrawText(TextFormat("Player Y speed: %.4f", player.phys.body.velocity.y), 10, 120, 20, GRAY);

    DrawText(TextFormat("Player alt X speed: %.4f", player.phys.body.altVelocity.x), 10, 160, 20, GRAY);
    DrawText(TextFormat("Player alt Y speed: %.4f", player.phys.body.altVelocity.y), 10, 180, 20, GRAY);

    DrawText(TextFormat("Player final X speed: %.4f", player.phys.body.GetFinalVelocity().x), 10, 220, 20, GRAY);
    DrawText(TextFormat("Player final Y speed: %.4f", player.phys.body.GetFinalVelocity().y), 10, 240, 20, GRAY);
}
