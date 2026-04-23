#include "level.h"

#include <iostream>
#include<cstring>

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

    if(dataSize != (ROWS * COLS * sizeof(Tile))) return;

    memcpy(level, fileData, dataSize);
    
    UnloadFileData(fileData);
}

void Level::InitLevel(const char *levelPath, float dt, int iterations)
{
    this->iterations = iterations;

    this->dt = dt;

    gravity = 3500;

    player.entityData.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    camera.zoom = 1.2f;

    float step = 1.0f / (float)gridSize;

    camera.zoom = roundf(camera.zoom / step) * step;

    ClearTileMatrix();

    ClearGameObjMatrix();

    ClearPlatformList();

    LoadLevelData(levelPath);

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            TileType type = level[i][j].type;

            if(IsTypeInvalid(type)) level[i][j].type = TileType::VOID;

            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

            if(type == TileType::PLAYER_SPAWN)
            {
                player.phys.position = {xpos, ypos};
                player.spawnPos = player.phys.position;
            }
           
            //platforms...

            bool isPlatform = type > TileType::PLATFORM_START && type < TileType::PLATFORM_END;

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

                platform->phys.mainAABB.width = platformWidth;
                platform->phys.mainAABB.height = platformHeight;

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

                    platform->phys.body.velocity.y = -platformSpeed;

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

            bool upLeft = IsTileEmptyInverted(i - 1, j - 1, level, TileType::SOLID);
            bool up = IsTileEmptyInverted(i, j - 1, level, TileType::SOLID);
            bool upRight = IsTileEmptyInverted(i + 1, j - 1, level, TileType::SOLID);

            bool right = IsTileEmptyInverted(i + 1, j, level, TileType::SOLID);
            bool downRight = IsTileEmptyInverted(i + 1, j + 1, level, TileType::SOLID);
            bool down = IsTileEmptyInverted(i, j + 1, level, TileType::SOLID);

            bool downLeft = IsTileEmptyInverted(i - 1, j + 1, level, TileType::SOLID);
            bool left = IsTileEmptyInverted(i, j - 1, level, TileType::SOLID);

            bool isEdge = upLeft || up || upRight ||
            right || downRight || down ||
            downLeft || left;

            if(!isEdge) continue;

            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

            level[i][j].gameObj = new GameObject();

            GameObject* objTile = level[i][j].gameObj;

            objTile->mainAABB = {0, 0, gridSize, gridSize};

            objTile->position = {xpos, ypos};

            objTile->UpdateAABB();

            float treadmillVel = 100.0f;

            switch (type)
            {
            case TileType::SOLID:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::GOAL:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = false;
            }
            break;

            case TileType::PLATFORM_STOP:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = true;
            }
            break;
            
            case TileType::TRAMPOLINE:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::GRAVITY_CHANGER:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = false;
            }
            break;

            case TileType::TREADMILL_RIGHT:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;

                objTile->body.velocity.x = treadmillVel;
            }
            break;

            case TileType::TREADMILL_LEFT:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;

                objTile->body.velocity.x = -treadmillVel;
            }
            break;

            case TileType::ONE_WAY_UP:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_DOWN:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_RIGHT:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_LEFT:
            {
                objTile->canEntityCollidePhysically = true;
                objTile->canPlatformCollidePhysically = true;
            }
            break;

            case TileType::SPIKE:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = true;

                SpriteRenderData* spikeRenderData = GetActiveRenderData(TileType::SPIKE);

                float widthFactor = 0.6f;
                float heightFactor = 0.4f;

                int orientation = 4;

                if(spikeRenderData)
                {
                    orientation = tile->textureIndex % spikeRenderData->maxFrames;
                }

                switch (orientation)
                {
                case 0:
                case 1:
                    break;

                case 2:
                case 3:
                    {
                        std::swap(widthFactor, heightFactor);
                    }
                    break;
                default:
                    {
                        float factor = 0.8f;

                        widthFactor = factor;
                        heightFactor = factor;
                    }
                    break;
                }

                objTile->mainAABB.width *= widthFactor; 
                objTile->mainAABB.height *= heightFactor;

                objTile->UpdateAABB();

                Vector2 subHitboxOffset = {0,0};

                Vector2 subHitboxSize = {objTile->mainAABB.height * 0.5f, objTile->mainAABB.width * 0.5f};

                float spikePositionCorrectionFactor = 0.6f;

                switch (orientation)
                {
                case 0: 
                {
                    objTile->mainAABB.y += objTile->mainAABB.height * spikePositionCorrectionFactor;

                    subHitboxOffset.y = -10;

                    objTile->AddSubHitbox(subHitboxOffset.x, subHitboxOffset.y, subHitboxSize.x, subHitboxSize.y);
                }
                    break;
                case 1:
                {
                    objTile->mainAABB.y -= objTile->mainAABB.height * spikePositionCorrectionFactor;

                    subHitboxOffset.y = 10;

                    objTile->AddSubHitbox(subHitboxOffset.x, subHitboxOffset.y, subHitboxSize.x, subHitboxSize.y);
                }
                    break;
                case 2:
                {
                    objTile->mainAABB.x += objTile->mainAABB.width * spikePositionCorrectionFactor;

                    subHitboxOffset.x = -10;

                    objTile->AddSubHitbox(subHitboxOffset.x, subHitboxOffset.y, subHitboxSize.x, subHitboxSize.y);
                }
                    break;
                case 3:
                {
                    objTile->mainAABB.x -= objTile->mainAABB.width * spikePositionCorrectionFactor;

                    subHitboxOffset.x = 10;

                    objTile->AddSubHitbox(subHitboxOffset.x, subHitboxOffset.y, subHitboxSize.x, subHitboxSize.y);
                }
                    break;
                default:
                    break;
                }
                
            }
            break;

            default:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = false;
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

    TileRange playerTileRange = CalculateTileRangeDirectional(
        player.phys.position,
        player.phys.body.velocity,
        collisionTileCheckRange
    );

    player.Update(dt, iterations);

    //X pass

    player.phys.UpdatePositionX(dt, iterations);

    for(int i = playerTileRange.startX; i != playerTileRange.endX; i+= playerTileRange.stepX)
    {
        for(int j = playerTileRange.startY; j != playerTileRange.endY; j+= playerTileRange.stepY)
        {
            GameObject* objTile = level[i][j].gameObj;            

            if(!objTile) continue;

            if(!objTile->canEntityCollidePhysically) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                /*SolveCollisions(
                    &player.phys, objTile, 
                    true, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );*/

                SolveCollisions_CCD_AxisSplit(&player.phys, objTile, true, dt);
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

    //Y pass

    player.phys.UpdatePositionY(dt, iterations);

    for(int i = playerTileRange.startX; i != playerTileRange.endX; i+= playerTileRange.stepX)
    {
        for(int j = playerTileRange.startY; j != playerTileRange.endY; j+= playerTileRange.stepY)
        {
            GameObject* objTile = level[i][j].gameObj;

            if(!objTile) continue;            

            if(!objTile->canEntityCollidePhysically) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                /*SolveCollisions(
                    &player.phys, objTile, 
                    false, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );*/

                SolveCollisions_CCD_AxisSplit(&player.phys, objTile, false, dt);
            }
            else if(IsOneWayUpDown(i, j))
            {
                SolveCollisionsOneWayUpDown
                (
                    &player.phys, objTile,
                    tile.type == TileType::ONE_WAY_UP,
                    isGravityUp,
                    false
                );
            }
        }
    }

    bool isPlayerGrounded = false;

    bool isPlayerFalling = player.phys.body.velocity.y >= 0;

    if(player.entityData.flipY) isPlayerFalling = player.phys.body.velocity.y <= 0;

    bool isPlayerTouchingGravityChanger = false;

    bool isPlayerTouchingSpike = false;

    //platforms

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(platform->IsInactive()) continue;

        if(platform->updateRequired) platform->Update(dt, iterations);

        /*SolveCollisionsOneWayUpDown(
            &player.phys, &platform->phys,
            true, isGravityUp, true
        );*/

        SolveCollisions_CCD_AxisSplit(&player.phys, &platform->phys, false, dt);

        if(CheckCollisionRecs(player.GetJumpDetector(), platform->phys.mainAABB) && isPlayerFalling)
        {
            if(!isGravityUp && IsAbove(player.phys.mainAABB, platform->phys.mainAABB, 0.0f) || (isGravityUp && IsBelow(player.phys.mainAABB, platform->phys.mainAABB, 0.0f)))
            {
                platform->updateRequired = true;
                isPlayerGrounded = true;
            }
        }

        bool isMovingPlatform = platform->isHorizontal || platform->isVertical;

        if(!isMovingPlatform) continue;

        TileRange platformRange = CalculateTileRangeDirectional(
            platform->phys.position,
            platform->phys.body.velocity,
            collisionTileCheckRange
        );

        for(int i = platformRange.startX; i != platformRange.endX; i+= platformRange.stepX)
        {
            for(int j = platformRange.startY; j != platformRange.endY; j += platformRange.stepY)
            {
                if(!level[i][j].gameObj) continue;

                SolveCollisions_Platform(&platform->phys, level[i][j].gameObj, platform->isHorizontal);
            }
        }
    }

    //tile triggers
    for(int i = playerTileRange.startX; i != playerTileRange.endX; i+= playerTileRange.stepX)
    {
        for(int j = playerTileRange.startY; j != playerTileRange.endY; j+= playerTileRange.stepY)
        {
            GameObject* objTile = level[i][j].gameObj;

            Tile& tile = level[i][j];

            if(!objTile) continue;

            if(IsOneWayRightLeft(i,j)) continue;

            if(tile.type == TileType::PLATFORM_STOP) continue;

            if(tile.type == TileType::GRAVITY_CHANGER)
            {
                if (CheckCollisionRecs(player.phys.mainAABB, objTile->mainAABB))
                    isPlayerTouchingGravityChanger = true;
            } 

            if(tile.type == TileType::SPIKE)
            {
                if (CheckCollisionRecs(player.phys.mainAABB, objTile->mainAABB))
                    isPlayerTouchingSpike = true;

                for(int h = 0; h < objTile->subAABBList.size(); h++)
                {
                    if(CheckCollisionRecs(player.phys.mainAABB, objTile->subAABBList[h]))
                        isPlayerTouchingSpike = true;
                }
            }   

            if(IsTileJumpTrigger(i,j)) continue;

            if(CheckCollisionRecs(player.GetJumpDetector(), objTile->mainAABB) && isPlayerFalling)
            {
                if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
                {
                    player.phys.body.altVelocity = objTile->body.velocity;
                }

                if(!IsOneWayUpDown(i,j)) isPlayerGrounded = true;
                else if(IsOneWayUpDown(i,j))
                {
                    if(tile.type == TileType::ONE_WAY_UP &&
                        IsAbove(player.phys.mainAABB, objTile->mainAABB, 0.0f)
                    )
                    {
                        isPlayerGrounded = true;
                    }
                    else if(tile.type == TileType::ONE_WAY_DOWN &&
                        IsBelow(player.phys.mainAABB, objTile->mainAABB, 0.0f)
                    )
                    {
                        isPlayerGrounded = true;
                    }
                }
            }
        }
    }

    if(!player.isTouchingGravityChanger && isPlayerTouchingGravityChanger)
    {
        player.canJump = false;
        player.isJumping = false;

        gravity *= -1;
        isGravityUp = gravity < 0;

        player.gravity = gravity;
        player.entityData.flipY = isGravityUp;

        for(int i = 0; i < platformList.size(); i++)
        {
            Platform*  platform = platformList[i];

            if(!platform) continue;

            if(!platform->isFalling || platform->updateRequired) continue;

            platform->gravity = gravity;
        }
    }

    if(!player.isTouchingSpike && isPlayerTouchingSpike)
    {
        if(gravity < 0) gravity *= -1;

        isGravityUp = gravity < 0;

        player.gravity = gravity;
        player.entityData.flipY = isGravityUp;

        for(int i = 0; i < platformList.size(); i++)
        {
            Platform*  platform = platformList[i];

            if(!platform) continue;

            if(!platform->isFalling || platform->updateRequired) continue;

            platform->gravity = gravity;
        }

        player.Respawn();
    }

    //booleans update
    player.canJump = isPlayerGrounded;

    player.isTouchingGravityChanger = isPlayerTouchingGravityChanger;

    player.isTouchingSpike = isPlayerTouchingSpike;
}

void Level::DrawLevel()
{
    BeginMode2D(camera);

    TileRange playerTileRange = CalculateTileRange(
        player.phys.position,
        renderTileCheckRange
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Color platformColor = HORIZONTAL_MOVING_PLATFORM;

        Platform* platform = platformList[i];

        if(!platform) continue;

        if(platform->isVertical) platformColor = VERTICAL_MOVING_PLATFORM;
        else if(platform->isFalling) platformColor = FALLING_PLATFORM;
        else if(platform->isDisappearing) platformColor = DISAPPEARING_PLATFORM;

        DrawRectangleRec(platform->phys.mainAABB, platformColor);
    }

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            Tile tile = level[i][j];

            if(IsNotRealTile(i,j)) continue;

            SpriteRenderData* tileRenderData = GetActiveRenderData(tile.type, tile.variantIndex);

            int frameToDraw = tile.textureIndex;

            if(tileRenderData)
            {
                if(tileRenderData->spacing != 1)
                {
                    int loopEnd = tile.textureIndex + tileRenderData->spacing - 1;

                    if(tileRenderData->spacing == 0) loopEnd = tileRenderData->endFrame;

                    frameToDraw = GetCurrentFrame(
                        tileRenderData->animationFrames,
                        tile.textureIndex,
                        loopEnd,
                        5.0f
                    );
                }
            }

            if(tileRenderData && tile.textureIndex >= 0 && tile.textureIndex < (int)tileRenderData->animationFrames.size())
            {
                DrawTile(tileRenderData, frameToDraw, {(float)i * gridSize, (float)j * gridSize}, gridSize);
            }
            else
            {
                Color color = GetTileColor(tile.type);

                if(IsColorOf(color, BLANK)) continue;

                if(tile.gameObj) DrawRectangleRec(tile.gameObj->mainAABB, color);
            }
        }
    }

    DrawSprite(
        player.characterRenderData,
        player.entityData,
        player.currentFrame
    );

    DrawSprite(
        player.weaponRenderData,
        player.entityData,
        player.currentFrame
    );

    DebugDrawing();

    EndMode2D();

    DebugTextDrawing();
}

void Level::DebugDrawing()
{
    TileRange playerTileRange = CalculateTileRange(
        player.phys.position,
        renderTileCheckRange
    );

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* tileObj = level[i][j].gameObj;

            if(!tileObj) continue;

            DrawAABB(tileObj->mainAABB, RED);

            for(int h = 0; h < tileObj->subAABBList.size(); h++)
            {
                DrawAABB(tileObj->subAABBList[h], MAGENTA);
            }
        }
    }

    Color gridColor = GRAY;
    gridColor.a = (int)(255 * 0.5f);

    for(int i = 0; i <= ROWS * gridSize; i+= gridSize)
    {
        DrawLine(i, 0, i, ROWS * gridSize, gridColor);
    }

    for(int j = 0; j <= COLS * gridSize; j+= gridSize)
    {
        DrawLine(0, j, COLS * gridSize, j, gridColor);
    }

    DrawAABB(player.phys.mainAABB, ORANGE);

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
