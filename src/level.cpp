#include "level.h"

#include <iostream>
#include <cstring>

Level::Level() : player({0, 0})
{
}

Level::~Level()
{
    ClearTileMatrix();

    ClearPlatformList();

    UnloadAssets();
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

    gravity = GRAVITY;

    player.entityData.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    camera.zoom = 1.2f;

    float step = 1.0f / (float)gridSize;

    camera.zoom = roundf(camera.zoom / step) * step;

    ClearTileMatrix();

    ClearPlatformList();

    LoadLevelData(levelPath);

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            Tile* tile = &level[i][j];
            TileType type = tile->type;

            if(IsTypeInvalid(type)) level[i][j].type = TileType::VOID;

            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

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

                platform->phys.transform.position = {xpos, ypos};

                platform->phys.hitboxes.push_back(Hitbox{{0,0}, {platformWidth, platformHeight}});

                platform->phys.UpdateHitboxes();

                float platformSpeed = 100.0f;

                platform->SetTimer(0.3f);

                platform->textureIndex = level[i][j].textureIndex;

                platform->variantIndex = level[i][j].variantIndex;

                switch (type)
                {
                case TileType::HORIZONALT_MOVING_PLATFORM:
                {
                    platform->type = PlatformType::MOVING_HORIZONTAL;

                    platform->phys.body->velocity.x = platformSpeed;

                    platform->updateRequired = true;
                }
                break;

                case TileType::VERTICAL_MOVING_PLATFORM:
                {
                    platform->type = PlatformType::MOVING_VERTICAL;

                    platform->phys.body->velocity.y = -platformSpeed;

                    platform->updateRequired = true;
                }
                break;

                case TileType::FALLING_PLATFORM:
                {
                    platform->type = PlatformType::FALLING;

                    platform->phys.body->hasGravity = true;
                }
                break;

                case TileType::DISAPPEARING_PLATFORM:
                {
                    platform->type = PlatformType::DISAPPEARING;
                }
                break;

                default:
                    break;
                }

                platformList.push_back(platform);
            }

            if(type == TileType::PLAYER_SPAWN)
            {
                player.phys.transform.position = {xpos, ypos};
                player.spawnPos = player.phys.transform.position;
            }

            //actual tiles
            if(IsNotRealTile(type)) continue;

            tile->gameObj.transform.scale = tileScale;

            tile->gameObj.transform.position = {xpos, ypos};

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

            tile->gameObj.body = new SimpleBody2D();

            tile->gameObj.hitboxes.push_back(Hitbox{{0,0}, {gridSize, gridSize}});

            tile->gameObj.UpdateHitboxes();

            float treadmillVel = 200.0f;

            switch (type)
            {
            case TileType::SOLID:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            case TileType::GOAL:
            {
                tile->gameObj.canEntityCollidePhysically = false;
                tile->gameObj.canPlatformCollidePhysically = false;
            }
            break;

            case TileType::PLATFORM_STOP:
            {
                tile->gameObj.canEntityCollidePhysically = false;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;
            
            case TileType::TRAMPOLINE:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            case TileType::GRAVITY_CHANGER:
            {
                tile->gameObj.canEntityCollidePhysically = false;
                tile->gameObj.canPlatformCollidePhysically = false;
            }
            break;

            case TileType::TREADMILL_RIGHT:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;

                tile->gameObj.body->velocity.x = treadmillVel;
            }
            break;

            case TileType::TREADMILL_LEFT:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;

                tile->gameObj.body->velocity.x = -treadmillVel;
            }
            break;

            case TileType::ONE_WAY_UP:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_DOWN:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_RIGHT:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            case TileType::ONE_WAY_LEFT:
            {
                tile->gameObj.canEntityCollidePhysically = true;
                tile->gameObj.canPlatformCollidePhysically = true;
            }
            break;

            default:
            {
                tile->gameObj.canEntityCollidePhysically = false;
                tile->gameObj.canPlatformCollidePhysically = false;
            }
            break;
            }

            if(IsTileSpike(tile->type))
            {
                tile->gameObj.canEntityCollidePhysically = false;
                tile->gameObj.canPlatformCollidePhysically = true;

                SpriteRenderData* spikeRenderData = GetTileActiveRenderData(TileType::SPIKE);

                int orientation = 4;

                if(spikeRenderData)
                {
                    int logicalIndex = tile->textureIndex / spikeRenderData->spacing;

                    int totalOrientations = spikeRenderData->maxFrames / spikeRenderData->spacing;

                    orientation = logicalIndex % totalOrientations;
                }

                auto AddSpikeHitbox = [&](float widthFactor, float heightFactor, float offsetX, float offsetY, float defaultFactor)
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

                    Vector2 size = {tile->gameObj.GetMainAABB()->width * widthFactor, tile->gameObj.GetMainAABB()->height * heightFactor};

                    Vector2 offset = {0,0};

                    offset.x = offsetX;
                    offset.y = offsetY;

                    switch (orientation)
                    {
                        case 1: offset.y = -offsetY; break;

                        case 2: offset = {offsetY, offsetX}; break;
                        case 3: offset = {-offsetY, offsetX}; break;

                        case 4: offset = {0,0}; break;
                    }

                    tile->gameObj.AddSubHitbox(offset, size);
                };

                switch (tile->type)
                {
                case TileType::SPIKE:
                {
                    float wFactor = 0.6f;
                    float hFactor = 0.4f;
  
                    float correctionY_A = 12;
                    float correctionY_B = -9;

                    AddSpikeHitbox(wFactor, hFactor,0 ,correctionY_A, 0.8f);
                    AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, 0 ,correctionY_B, 0.8f);
                }
                    break;

                case TileType::SPIKE_DOUBLE:
                {
                    float wFactor = 0.3f;
                    float hFactor = 0.2f;

                    float correctionX_A = 13;

                    float correctionX_B = -correctionX_A;

                    float correctionY_A = 15;

                    float correctionY_B = 5;

                    AddSpikeHitbox(wFactor, hFactor,correctionX_A, correctionY_A, 0.6f);
                    AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f,correctionX_A, correctionY_B, 0.6f);

                    AddSpikeHitbox(wFactor, hFactor,correctionX_B, correctionY_A, 0.6f);
                    AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f,correctionX_B, correctionY_B, 0.6f);
                }
                break;

                case TileType::SPIKE_SMALL:
                {
                    float wFactor = 0.3f;
                    float hFactor = 0.2f;

                    float correctionY_A = 15;

                    float correctionY_B = 5;

                    AddSpikeHitbox(wFactor, hFactor, 0, correctionY_A, 0.6f);
                    AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, 0, correctionY_B, 0.6f);
                }
                break;

                default: break;
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

    UpdateCamera(player.phys.transform.position, {0, -100});
}

void Level::DiscreteUpdate()
{
    bool isGravityUp = gravity < 0;

    TileRange playerTileRange = CalculateTileRange(
        player.phys.transform.position.x,
        player.phys.transform.position.y,
        collisionTileCheckRange
    );

    player.ResetFalgs();

    player.Update(dt, iterations);

    //X pass

    player.phys.UpdatePositionX(dt, iterations);

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject objTile = level[i][j].gameObj;

            if(!objTile.body || objTile.hitboxes.empty()) continue;

            if(!objTile.canEntityCollidePhysically) continue;

            if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetMainAABB())) continue;

            const Tile& tile = level[i][j];

            if(!IsTileOneWay(tile.type))
            {
                SolveCollisions(
                    &player.phys, &objTile, 
                    true, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );
            }
            else if(IsOneWayRightLeft(tile.type))
            {
                SolveCollisionsOneWayLeftRight(
                    &player.phys, &objTile,
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
            GameObject objTile = level[i][j].gameObj;

            if(!objTile.body || objTile.hitboxes.empty()) continue;            

            if(!objTile.canEntityCollidePhysically) continue;

            if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetMainAABB())) continue;

            const Tile& tile = level[i][j];

            if(!IsTileOneWay(tile.type))
            {
                SolveCollisions(
                    &player.phys, &objTile, 
                    false, isGravityUp, 
                    tile.type == TileType::TRAMPOLINE,
                    false
                );
            }
            else if(IsOneWayUpDown(tile.type))
            {
                SolveCollisionsOneWayUpDown
                (
                    &player.phys, &objTile,
                    tile.type == TileType::ONE_WAY_UP,
                    isGravityUp,
                    false
                );
            }
        }
    }

    //platforms

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(IsPlatformFarFromPlayer(platform->phys.transform.position)) continue;

        if(platform->IsInactive()) continue;

        if(platform->updateRequired) platform->Update(dt, iterations);

        SolveCollisionsOneWayUpDown(
            &player.phys, &platform->phys,
            true, isGravityUp, true
        );

        if(CheckCollisionRecs(player.GetJumpDetector(), *platform->phys.GetMainAABB()) && player.IsFalling())
        {
            if(!isGravityUp && IsAbove(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f) || (isGravityUp && IsBelow(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f)))
            {
                platform->updateRequired = true;
                player.wasGrounded = true;
            }
        }

        bool isMovingPlatform = platform->type == PlatformType::MOVING_HORIZONTAL || platform->type == PlatformType::MOVING_VERTICAL;

        if(!isMovingPlatform) continue;

        TileRange platformRange = CalculateTileRange(
            platform->phys.transform.position.x,
            platform->phys.transform.position.y,
            collisionTileCheckRange
        );

        for(int i = platformRange.startX; i <= platformRange.endX; i++)
        {
            for(int j = platformRange.startY; j <= platformRange.endY; j++)
            {
                GameObject objTile = level[i][j].gameObj;

                if(!objTile.body || objTile.hitboxes.empty()) continue;

                if(!objTile.canPlatformCollidePhysically) continue;

                SolveCollisions_Platform(&platform->phys, &level[i][j].gameObj, platform->type == PlatformType::MOVING_HORIZONTAL);
            }
        }
    }

    //tile triggers and non solid tiles
    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            Tile& tile = level[i][j];

            if(IsNotRealTile(tile.type)) continue;

            GameObject objTile = tile.gameObj;

            if(objTile.hitboxes.empty()) continue;

            if(IsOneWayRightLeft(tile.type)) continue;

            if(tile.type == TileType::PLATFORM_STOP) continue;

            if(CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetMainAABB()))
            {
                if(tile.type == TileType::GRAVITY_CHANGER) player.wasTouchingGravityChanger = true;

                if(IsTileSpike(tile.type))
                {
                    for(int h = 0; h < objTile.hitboxes.size(); h++)
                    {
                        if(CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetSubAABB(h)))
                            player.wasTouchingSpike = true;
                    }
                }
                
                if(IsTileWind(tile.type) && !player.windApplied)
                {
                    bool isEdgeUp = tile.type == TileType::WIND_UP && IsTileEmpty(i, j - 1, level, TileType::VOID);

                    bool isEdgeDown = tile.type == TileType::WIND_DOWN && IsTileEmpty(i, j + 1, level, TileType::VOID);

                    ApplyWind(
                        &player.phys,
                        &objTile,
                        tile.type == TileType::WIND_UP,
                        tile.type == TileType::WIND_DOWN,
                        tile.type == TileType::WIND_LEFT,
                        tile.type == TileType::WIND_RIGHT,
                        isEdgeUp,
                        isEdgeDown,
                        isGravityUp
                    );

                    player.windApplied = true;
                }

                if(tile.type == TileType::WATER && !player.inWater)
                {
                    ApplyWaterPhysics(&player.phys, isGravityUp);

                    player.inWater = true;
                }
            }

            if(IsTileNotJumpTrigger(tile.type)) continue;

            if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
            {
                if(CheckCollisionRecs(player.GetTreadmillDetector(), *objTile.GetMainAABB()) && player.IsFalling())
                    player.phys.body->altVelocity = objTile.body->velocity;
            }

            if(CheckCollisionRecs(player.GetJumpDetector(), *objTile.GetMainAABB()) && player.IsFalling())
            {
                if(!IsOneWayUpDown(tile.type)) player.wasGrounded = true;
                else if(IsOneWayUpDown(tile.type))
                {
                    if(tile.type == TileType::ONE_WAY_UP &&
                        IsAbove(*player.phys.GetMainAABB(), *objTile.GetMainAABB(), 0.0f)
                    )
                    {
                        player.wasGrounded = true;
                    }
                    else if(tile.type == TileType::ONE_WAY_DOWN &&
                        IsBelow(*player.phys.GetMainAABB(), *objTile.GetMainAABB(), 0.0f)
                    )
                    {
                        player.wasGrounded = true;
                    }
                }
            }
        }
    }

    if(!player.isTouchingGravityChanger && player.wasTouchingGravityChanger)
    {
        player.isGrounded = false;
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

    if(!player.isTouchingSpike && player.wasTouchingSpike)
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

    player.UpdateFlags();
}

void Level::DrawLevel()
{
    BeginMode2D(camera);

    TileRange playerTileRange = CalculateTileRange(
        player.phys.transform.position.x,
        player.phys.transform.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(IsPlatformFarFromPlayer(platform->phys.transform.position)) continue;

        PlatformType platformType = platform->type;

        SpriteRenderData* platformRenderData =  GetPlatformActiveRenderData(platformType, platform->variantIndex);

        if(platformRenderData)
        {
            int frameToDraw = platform->textureIndex;

            if(platformRenderData->spacing != 1)
            {
                frameToDraw = GetCurrentFrame(
                    platformRenderData->animationFrames,
                    platform->textureIndex,
                    platformRenderData->spacing,
                    platformRenderData->animationSpeed
                );
            }

            if(frameToDraw >= 0 && frameToDraw < (int)platformRenderData->animationFrames.size())
            {
                DrawTile(platformRenderData, frameToDraw, platform->phys.transform);
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
                    frameToDraw = GetCurrentFrame(
                        tileRenderData->animationFrames,
                        tile.textureIndex,
                        tileRenderData->spacing,
                        tileRenderData->animationSpeed
                    );
                }

                if(frameToDraw >= 0 && frameToDraw < (int)tileRenderData->animationFrames.size())
                {
                    DrawTile(tileRenderData, frameToDraw, tile.gameObj.transform);
                }
            }
            else
            {
                Color color = GetTileColor(tile.type);

                if(IsColorOf(color, BLANK)) continue;

                if(!tile.gameObj.hitboxes.empty()) DrawRectangleRec(*tile.gameObj.GetMainAABB(), color);
                else DrawRectangle(i * gridSize, j * gridSize, gridSize, gridSize, color);
            }
        }
    }

    DrawSprite(
        player.phys.transform,
        player.characterRenderData,
        player.entityData,
        player.currentFrame
    );

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawBullet(b->posititon.x, b->posititon.y, b->radius, b->mainColor, b->backColor);
    }

    if(!player.bulletpool->explosions.empty())
    {
        for(int i = 0; i < player.bulletpool->activeExplosions.size(); i++)
        {
            Explosion* e = player.bulletpool->activeExplosions[i];

            if(!e) continue;

            DrawExplosion(e->position.x, e->position.y, e->radius, e->renderData, 0, tileScale);
        }
    }

    DrawSprite(
        player.phys.transform,
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
        player.phys.transform.position.x,
        player.phys.transform.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(!IsPlatformFarFromPlayer(platform->phys.transform.position))
        {
            DrawLine(
                player.phys.transform.position.x, player.phys.transform.position.y, 
                platform->phys.transform.position.x, platform->phys.transform.position.y, 
                RED
            );
        }

        DrawAABB(*platform->phys.GetMainAABB(), RED);
    }

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject tileObj = level[i][j].gameObj;

            if(tileObj.hitboxes.empty()) continue;

            DrawAABB(*tileObj.GetMainAABB(), RED);

            for(int h = 0; h < tileObj.hitboxes.size(); h++)
            {
                DrawAABB(*tileObj.GetSubAABB(h), MAGENTA);
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

    DrawText(TextFormat("Player X speed: %.4f", player.phys.body->velocity.x), 10, 100, 20, GRAY);
    DrawText(TextFormat("Player Y speed: %.4f", player.phys.body->velocity.y), 10, 120, 20, GRAY);

    DrawText(TextFormat("Player alt X speed: %.4f", player.phys.body->altVelocity.x), 10, 160, 20, GRAY);
    DrawText(TextFormat("Player alt Y speed: %.4f", player.phys.body->altVelocity.y), 10, 180, 20, GRAY);

    DrawText(TextFormat("Player final X speed: %.4f", player.phys.body->GetFinalVelocity().x), 10, 220, 20, GRAY);
    DrawText(TextFormat("Player final Y speed: %.4f", player.phys.body->GetFinalVelocity().y), 10, 240, 20, GRAY);
}
