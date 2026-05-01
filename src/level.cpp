#include "level.h"

#include <iostream>
#include <cstring>

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

                platform->phys.mainHitbox = {{0,0}, {platformWidth, platformHeight}};

                platform->phys.UpdateAABB();

                float platformSpeed = 100.0f;

                platform->SetTimer(0.3f);

                platform->textureIndex = level[i][j].textureIndex;

                platform->variantIndex = level[i][j].variantIndex;

                if(type == TileType::HORIZONALT_MOVING_PLATFORM)
                {
                    platform->type = PlatformType::MOVING_HORIZONTAL;

                    platform->phys.body.velocity.x = platformSpeed;

                    platform->updateRequired = true;
                }
                else if(type == TileType::VERTICAL_MOVING_PLATFORM)
                {
                    platform->type = PlatformType::MOVING_VERTICAL;

                    platform->phys.body.velocity.y = -platformSpeed;

                    platform->updateRequired = true;
                }
                else if(type == TileType::FALLING_PLATFORM)
                {                    
                    platform->type = PlatformType::FALLING;

                    platform->phys.body.hasGravity = true;
                }
                else if(type == TileType::DISAPPEARING_PLATFORM)
                {
                    platform->type = PlatformType::DISAPPEARING;
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

            if(IsNotRealTile(type)) continue;

            //decorational tiles don't need a physical body
            if(level[i][j].type == TileType::DECO) continue;

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

            objTile->mainHitbox = {{0,0}, {gridSize, gridSize}};

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

            default:
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = false;
            }
            break;
            }

            if(IsTileSpike(tile->type))
            {
                objTile->canEntityCollidePhysically = false;
                objTile->canPlatformCollidePhysically = true;

                SpriteRenderData* spikeRenderData = GetTileActiveRenderData(TileType::SPIKE);

                int orientation = 4;

                if(spikeRenderData)
                {
                    int logicalIndex = tile->textureIndex / spikeRenderData->spacing;

                    int totalOrientations = spikeRenderData->maxFrames / spikeRenderData->spacing;

                    orientation = logicalIndex % totalOrientations;
                }

                auto AddSpikeHitbox = [&](float widthFactor, float heightFactor, float correctionFactor, float defaultFactor)
                {
                    if(orientation == 2 || orientation == 3)
                    {
                        std::swap(widthFactor, heightFactor);
                    }
                    else if(orientation >= 4)
                    {
                        widthFactor = defaultFactor;
                        heightFactor = defaultFactor;
                    }

                    Vector2 size = {objTile->GetMainAABB()->width * widthFactor, objTile->GetMainAABB()->height * heightFactor};

                    Vector2 offset = {0,0};

                    float correctionX = objTile->GetMainAABB()->width * correctionFactor;
                    float correctionY = objTile->GetMainAABB()->height * correctionFactor;

                    switch (orientation)
                    {
                        case 0: offset.y = correctionY; break;
                        case 1: offset.y = -correctionY; break;
                        case 2: offset.x = correctionX; break;
                        case 3: offset.x = -correctionX; break;
                    }

                    objTile->AddSubHitbox(offset, size);
                };

                switch (tile->type)
                {
                case TileType::SPIKE:
                {
                    float wFactor = 0.6f;
                    float hFactor = 0.4f;

                    AddSpikeHitbox(wFactor, hFactor, 0.25, 0.8f);
                    AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, -0.2, 0.8f);
                }
                    break;
                
                default:
                    break;
                }
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

    player.bulletpool.get()->UpdateBullets(dt);

    player.Shoot(dt);

    UpdateCamera(player.phys.position, {0, -100});
}

void Level::DiscreteUpdate()
{
    bool isGravityUp = gravity < 0;

    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        collisionTileCheckRange
    );

    player.Update(dt, iterations);

    //X pass

    player.phys.UpdatePositionX(dt, iterations);

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = level[i][j].gameObj;            

            if(!objTile) continue;

            if(!objTile->canEntityCollidePhysically) continue;

            if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile->GetMainAABB())) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                SolveCollisions(
                    &player.phys, objTile, 
                    true, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );
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

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = level[i][j].gameObj;

            if(!objTile) continue;            

            if(!objTile->canEntityCollidePhysically) continue;

            if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile->GetMainAABB())) continue;

            const Tile& tile = level[i][j];

            if(!IsOneWayTile(i, j))
            {
                SolveCollisions(
                    &player.phys, objTile, 
                    false, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );
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

        if(IsPlatformFarFromPlayer(platform->phys.position)) continue;

        if(platform->IsInactive()) continue;

        if(platform->updateRequired) platform->Update(dt, iterations);

        SolveCollisionsOneWayUpDown(
            &player.phys, &platform->phys,
            true, isGravityUp, true
        );

        if(CheckCollisionRecs(player.GetJumpDetector(), *platform->phys.GetMainAABB()) && isPlayerFalling)
        {
            if(!isGravityUp && IsAbove(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f) || (isGravityUp && IsBelow(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f)))
            {
                platform->updateRequired = true;
                isPlayerGrounded = true;
            }
        }

        bool isMovingPlatform = platform->type == PlatformType::MOVING_HORIZONTAL || platform->type == PlatformType::MOVING_VERTICAL;

        if(!isMovingPlatform) continue;

        TileRange platformRange = CalculateTileRange(
            platform->phys.position.x,
            platform->phys.position.y,
            collisionTileCheckRange
        );

        for(int i = platformRange.startX; i <= platformRange.endX; i++)
        {
            for(int j = platformRange.startY; j <= platformRange.endY; j++)
            {
                if(!level[i][j].gameObj) continue;

                SolveCollisions_Platform(&platform->phys, level[i][j].gameObj, platform->type == PlatformType::MOVING_HORIZONTAL);
            }
        }
    }

    //tile triggers
    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = level[i][j].gameObj;

            Tile& tile = level[i][j];

            if(!objTile) continue;

            if(IsOneWayRightLeft(i,j)) continue;

            if(tile.type == TileType::PLATFORM_STOP) continue;

            if(CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile->GetMainAABB()))
            {
                if(tile.type == TileType::GRAVITY_CHANGER) isPlayerTouchingGravityChanger = true;

                if(IsTileSpike(tile.type))
                {
                    for(int h = 0; h < objTile->subHitboxList.size(); h++)
                    {
                        if(CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile->GetSubAABB(h)))
                            isPlayerTouchingSpike = true;
                    }
                }   
            }

            if(IsTileNotJumpTrigger(i,j)) continue;

            if(CheckCollisionRecs(player.GetJumpDetector(), *objTile->GetMainAABB()) && isPlayerFalling)
            {
                if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
                {
                    player.phys.body.altVelocity = objTile->body.velocity;
                }

                if(!IsOneWayUpDown(i,j)) isPlayerGrounded = true;
                else if(IsOneWayUpDown(i,j))
                {
                    if(tile.type == TileType::ONE_WAY_UP &&
                        IsAbove(*player.phys.GetMainAABB(), *objTile->GetMainAABB(), 0.0f)
                    )
                    {
                        isPlayerGrounded = true;
                    }
                    else if(tile.type == TileType::ONE_WAY_DOWN &&
                        IsBelow(*player.phys.GetMainAABB(), *objTile->GetMainAABB(), 0.0f)
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

            if(!(platform->type == PlatformType::FALLING) || platform->updateRequired) continue;

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

            if(!(platform->type == PlatformType::FALLING) || platform->updateRequired) continue;

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
        player.phys.position.x,
        player.phys.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(IsPlatformFarFromPlayer(platform->phys.position)) continue;

        PlatformType platformType = platform->type;

        SpriteRenderData* platformRenderData =  GetPlatformActiveRenderData(platformType, platform->variantIndex);

        if(platformRenderData)
        {
            int frameToDraw = platform->textureIndex;

            if(platformRenderData->spacing != 1)
            {
                int loopEnd = platform->textureIndex + platformRenderData->spacing - 1;

                    if(platformRenderData->spacing == 0) loopEnd = platformRenderData->endFrame;

                    frameToDraw = GetCurrentFrame(
                        platformRenderData->animationFrames,
                        platform->textureIndex,
                        loopEnd,
                        platformRenderData->animationSpeed
                    );
            }

            if(platform->textureIndex >= 0 && platform->textureIndex < (int)platformRenderData->animationFrames.size())
            {
                DrawTile(platformRenderData, frameToDraw, platform->phys.position, tileScale);
            }
        }
        else
        {
            Color platformColor = DECO;

            if(platformType == PlatformType::MOVING_VERTICAL) platformColor = VERTICAL_MOVING_PLATFORM;
            else if(platformType == PlatformType::MOVING_HORIZONTAL) platformColor = HORIZONTAL_MOVING_PLATFORM;
            else if(platformType == PlatformType::FALLING) platformColor = FALLING_PLATFORM;
            else if(platformType == PlatformType::DISAPPEARING) platformColor = DISAPPEARING_PLATFORM;

            DrawRectangleRec(*platform->phys.GetMainAABB(), platformColor);
        }
    }

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            Tile tile = level[i][j];

            if(IsNotRealTile(tile.type)) continue;

            SpriteRenderData* tileRenderData = GetTileActiveRenderData(tile.type, tile.variantIndex);

            if(tileRenderData)
            {
                int frameToDraw = tile.textureIndex;

                if(tileRenderData->spacing != 1)
                {
                    int loopEnd = tile.textureIndex + tileRenderData->spacing - 1;

                    if(tileRenderData->spacing == 0) loopEnd = tileRenderData->endFrame;

                    frameToDraw = GetCurrentFrame(
                        tileRenderData->animationFrames,
                        tile.textureIndex,
                        loopEnd,
                        tileRenderData->animationSpeed
                    );
                }

                if(tile.textureIndex >= 0 && tile.textureIndex < (int)tileRenderData->animationFrames.size())
                {
                    DrawTile(tileRenderData, frameToDraw, GetTileCenter(i,j), tileScale);
                }
            }
            else
            {
                Color color = GetTileColor(tile.type);

                if(IsColorOf(color, BLANK)) continue;

                if(tile.gameObj) DrawRectangleRec(*tile.gameObj->GetMainAABB(), color);
                else DrawRectangle(i * gridSize, j * gridSize, gridSize, gridSize, color);
            }
        }
    }

    DrawSprite(
        player.phys.position,
        player.characterRenderData,
        player.entityData,
        tileScale,
        player.currentFrame
    );

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawBullet(b->posititon.x, b->posititon.y, b->radius, b->mainColor, b->backColor);
    }

    DrawSprite(
        player.phys.position,
        player.weaponRenderData,
        player.entityData,
        tileScale,
        player.currentFrame
    );

    DebugDrawing();

    EndMode2D();

    //DebugTextDrawing();
}

void Level::DebugDrawing()
{
    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(!IsPlatformFarFromPlayer(platform->phys.position))
        {
            DrawLine(player.phys.position.x, player.phys.position.y, platform->phys.position.x, platform->phys.position.y, RED);
        }

        DrawAABB(*platform->phys.GetMainAABB(), RED);
    }

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* tileObj = level[i][j].gameObj;

            if(!tileObj) continue;

            DrawAABB(*tileObj->GetMainAABB(), RED);

            for(int h = 0; h < tileObj->subHitboxList.size(); h++)
            {
                DrawAABB(*tileObj->GetSubAABB(h), MAGENTA);
            }
        }
    }

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawCircleLines(b->posititon.x, b->posititon.y, b->radius, RED);
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

    DrawAABB(*player.phys.GetMainAABB(), ORANGE);

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
