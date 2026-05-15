#include "level.h"

#include <iostream>

Level::Level() : player({0, 0})
{
}

Level::~Level()
{
    ClearTileMatrix();

    ClearPlatformList();

    UnloadAssets();
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

    LoadLevelData(levelPath, level);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLS; j++)
            {
                Tile* tile = &level[l][i][j];
                TileType type = tile->type;

                if(type == TileType::PLAYER_SPAWN)
                {
                    player.phys.transform.position = tile->gameObj.transform.position;
                    player.spawnPos = player.phys.transform.position;
                }

                if(IsTypeInvalid(type)) level[l][i][j].type = TileType::VOID;

                //platforms...

                bool isPlatform = type > TileType::PLATFORM_START && type < TileType::PLATFORM_END;

                if(isPlatform)
                {
                    Platform* platform = new Platform();

                    float platformWidth = gridSize;
                    float platformHeight = gridSize;

                    platform->gravity = gravity;

                    switch (type)
                    {
                    case TileType::VERTICAL_MOVING_PLATFORM:
                    case TileType::HORIZONALT_MOVING_PLATFORM:
                    {
                        platformWidth = gridSize * 3.0f;
                        platformHeight = gridSize * 0.3f;
                    }
                    break;
                    
                    default: break;
                    }

                    platform->phys.transform = tile->gameObj.transform;
                    platform->phys.data = tile->gameObj.data;
                    platform->phys.direction = tile->gameObj.direction;

                    platform->ogPosition = platform->phys.transform.position;

                    platform->phys.hitboxes.push_back(Hitbox{{0,0}, {platformWidth, platformHeight}});

                    platform->phys.UpdateHitboxes();

                    float platformSpeed = 100.0f;

                    platform->SetTimer(0.3f);

                    platform->SetRespawnTimer(3.0f);

                    platform->textureIndex = level[l][i][j].textureIndex;

                    platform->variantIndex = level[l][i][j].variantIndex;

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

                    case TileType::VERTICAL_MOVING_SPIKE:
                    {
                        platform->type = PlatformType::MOVING_SPIKE_VERTICAL;

                        platform->phys.body->velocity.y = -platformSpeed;

                        platform->updateRequired = true;

                        float factor = 0.8f;

                        platform->phys.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    case TileType::HORIZONTAL_MOVING_SPIKE:
                    {
                        platform->type = PlatformType::MOVING_SPIKE_HORIZONTAL;

                        platform->phys.body->velocity.x = platformSpeed;

                        platform->updateRequired = true;

                        float factor = 0.8f;

                        platform->phys.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    default: break;
                    }

                    platformList.push_back(platform);
                }

                //actual tiles
                if(IsNotRealTile(type))
                {
                    if(tile->type != TileType::VOID) tile->type == TileType::VOID;

                    continue;
                }

                tile->gameObj.transform.scale = tileScale;

                //decorational tiles don't need a physical body
                if(level[l][i][j].type == TileType::DECO) continue;

                bool upLeft = IsTileEmptyInverted(l, i - 1, j - 1, level, TileType::SOLID);
                bool up = IsTileEmptyInverted(l, i, j - 1, level, TileType::SOLID);
                bool upRight = IsTileEmptyInverted(l, i + 1, j - 1, level, TileType::SOLID);

                bool right = IsTileEmptyInverted(l, i + 1, j, level, TileType::SOLID);
                bool downRight = IsTileEmptyInverted(l, i + 1, j + 1, level, TileType::SOLID);
                bool down = IsTileEmptyInverted(l, i, j + 1, level, TileType::SOLID);

                bool downLeft = IsTileEmptyInverted(l, i - 1, j + 1, level, TileType::SOLID);
                bool left = IsTileEmptyInverted(l, i, j - 1, level, TileType::SOLID);

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

                case TileType::ONE_WAY:
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

                    auto AddSpikeHitbox = [&](float widthFactor, float heightFactor, float offsetX, float offsetY)
                    {
                        if(tile->gameObj.direction == Direction::LEFT || tile->gameObj.direction == Direction::RIGHT)
                        {
                            std::swap(widthFactor, heightFactor);
                        }

                        Vector2 size = {tile->gameObj.GetMainAABB()->width * widthFactor, tile->gameObj.GetMainAABB()->height * heightFactor};

                        Vector2 offset = {0,0};

                        offset.x = offsetX;
                        offset.y = offsetY;

                        switch (tile->gameObj.direction)
                        {
                            case Direction::DOWN: offset.y = -offsetY; break;

                            case Direction::LEFT: offset = {offsetY, offsetX}; break;
                            case Direction::RIGHT: offset = {-offsetY, offsetX}; break;

                            default: break;
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

                        AddSpikeHitbox(wFactor, hFactor,0 ,correctionY_A);
                        AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, 0 ,correctionY_B);
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

                        AddSpikeHitbox(wFactor, hFactor,correctionX_A, correctionY_A);
                        AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f,correctionX_A, correctionY_B);

                        AddSpikeHitbox(wFactor, hFactor,correctionX_B, correctionY_A);
                        AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f,correctionX_B, correctionY_B);
                    }
                    break;

                    case TileType::SPIKE_SMALL:
                    {
                        float wFactor = 0.3f;
                        float hFactor = 0.2f;

                        float correctionY_A = 15;

                        float correctionY_B = 5;

                        AddSpikeHitbox(wFactor, hFactor, 0, correctionY_A);
                        AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, 0, correctionY_B);
                    }
                    break;

                    default: break;
                    }
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

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                GameObject& objTile = level[l][i][j].gameObj;

                if(!objTile.body || objTile.hitboxes.empty()) continue;

                if(!objTile.canEntityCollidePhysically) continue;

                if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetMainAABB())) continue;

                const Tile& tile = level[l][i][j];

                if(!IsTileOneWay(tile))
                {
                    SolveCollisions(
                        &player.phys, &objTile, 
                        true, isGravityUp, 
                        tile.type == TileType::TRAMPOLINE,
                        false
                    );
                }
                else if(IsOneWayRightLeft(tile))
                {
                    SolveCollisionsOneWayLeftRight(
                        &player.phys, &objTile,
                        tile.gameObj.direction == Direction::RIGHT
                    );
                }
            }
        }
    }
    

    //Y pass

    player.phys.UpdatePositionY(dt, iterations);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                GameObject& objTile = level[l][i][j].gameObj;

                const Tile& tile = level[l][i][j];

                if(IsNotRealTile(tile.type)) continue;

                if(!objTile.body || objTile.hitboxes.empty()) continue;

                if(!CheckCollisionRecs(*player.phys.GetMainAABB(), *objTile.GetMainAABB())) continue;

                if(objTile.canEntityCollidePhysically)
                {
                    if(!IsTileOneWay(tile))
                    {
                        SolveCollisions(
                            &player.phys, &objTile, 
                            false, isGravityUp, 
                            tile.type == TileType::TRAMPOLINE,
                            false
                        );
                    }
                    else if(IsOneWayUpDown(tile))
                    {
                        SolveCollisionsOneWayUpDown
                        (
                            &player.phys, &objTile,
                            tile.gameObj.direction == Direction::UP,
                            isGravityUp,
                            false
                        );
                    }
                }

                //tile triggers and non solid tiles

                //if bugs with tirggers happen move this back into its own loop
                
                if(IsOneWayRightLeft(tile)) continue;

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
                    
                    if(tile.type == TileType::WIND && !player.windApplied)
                    {
                        bool isEdgeUp = tile.gameObj.direction == Direction::UP && IsTileEmpty(l, i, j - 1, level, TileType::VOID);

                        bool isEdgeDown = tile.gameObj.direction == Direction::DOWN && IsTileEmpty(l, i, j + 1, level, TileType::VOID);

                        ApplyWind(
                            &player.phys,
                            &objTile,
                            tile.gameObj.direction,
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

                if(IsTileNotJumpTrigger(tile)) continue;

                if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
                {
                    if(CheckCollisionRecs(player.GetTreadmillDetector(), *objTile.GetMainAABB()) && player.IsFalling())
                        player.phys.body->altVelocity = objTile.body->velocity;
                }

                if(CheckCollisionRecs(player.GetJumpDetector(), *objTile.GetMainAABB()) && player.IsFalling())
                {
                    if(!IsOneWayUpDown(tile)) player.wasGrounded = true;
                    else if(IsOneWayUpDown(tile))
                    {
                        if(tile.gameObj.direction == Direction::UP &&
                            IsAbove(*player.phys.GetMainAABB(), *objTile.GetMainAABB(), 0.0f)
                        )
                        {
                            player.wasGrounded = true;
                        }
                        else if(tile.gameObj.direction == Direction::DOWN &&
                            IsBelow(*player.phys.GetMainAABB(), *objTile.GetMainAABB(), 0.0f)
                        )
                        {
                            player.wasGrounded = true;
                        }
                    }
                }
            }
        }
    }

    //platforms

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(platform->IsInactive())
        {
            platform->UpdateInactive(dt, iterations);
            continue;
        }

        bool isMovingPlatform = platform->type > PlatformType::MOVING_START && platform->type < PlatformType::MOVING_END;

        bool isRespawnPlatform = platform->type == PlatformType::FALLING || platform->type == PlatformType::DISAPPEARING;

        //moving platforms update culling
        if(IsPlatformFarFromPlayer(platform->phys.transform.position, MAX_DISTANCE_PLATFORM_PLAYER_SQR * 5) && isMovingPlatform) continue;

        //platforms update culling
        if(IsPlatformFarFromPlayer(platform->phys.transform.position) && !isRespawnPlatform && !isMovingPlatform) continue;

        if(platform->updateRequired) platform->Update(dt, iterations);

        if(!IsPlatformSpike(platform->type))
        {
            SolveCollisionsOneWayUpDown(
                &player.phys, &platform->phys,
                true, isGravityUp, true
            );
        }

        if(CheckCollisionRecs(player.GetJumpDetector(), *platform->phys.GetMainAABB()) && player.IsFalling())
        {
            if(!isGravityUp && IsAbove(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f) || 
            (isGravityUp && IsBelow(*player.phys.GetMainAABB(), *platform->phys.GetMainAABB(), 0.0f)))
            {
                platform->updateRequired = true;
                player.wasGrounded = true;
            }
        }

        for(int h = 1; h < platform->phys.hitboxes.size(); h++)
        {
            if(CheckCollisionRecs(*player.phys.GetMainAABB(), *platform->phys.GetSubAABB(h)))
            {
                if(IsPlatformSpike(platform->type))
                {
                    player.wasTouchingSpike = true;
                }
            }
        }

        if(!isMovingPlatform) continue;

        TileRange platformRange = CalculateTileRange(
            platform->phys.transform.position.x,
            platform->phys.transform.position.y,
            collisionTileCheckRange
        );

        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = platformRange.startX; i <= platformRange.endX; i++)
            {
                for(int j = platformRange.startY; j <= platformRange.endY; j++)
                {
                    GameObject& objTile = level[l][i][j].gameObj;

                    if(!objTile.body || objTile.hitboxes.empty()) continue;

                    if(!objTile.canPlatformCollidePhysically) continue;

                    if(!CheckCollisionRecs(*platform->phys.GetMainAABB(), *objTile.GetMainAABB())) continue;

                    SolveCollisions_Platform(&platform->phys, &level[l][i][j].gameObj, (platform->type > PlatformType::MOVING_X && platform->type < PlatformType::MOVING_Y));
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

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

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
                        DrawSprite(tile.gameObj, tileRenderData, frameToDraw);
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
    }

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform* platform = platformList[i];

        if(!platform) continue;

        if(IsPlatformFarFromPlayer(platform->phys.transform.position)) continue;

        PlatformType& platformType = platform->type;

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
                DrawSprite(platform->phys, platformRenderData, frameToDraw);
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

    DrawSprite(
        player.phys.transform,
        &player.characterRenderData,
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
        &player.weaponRenderData,
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

        for(int h = 1; h < platform->phys.hitboxes.size(); h++)
        {
            DrawAABB(*platform->phys.GetSubAABB(h), MAGENTA);
        }
    }

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                GameObject tileObj = level[l][i][j].gameObj;

                if(tileObj.hitboxes.empty()) continue;

                DrawAABB(*tileObj.GetMainAABB(), RED);

                for(int h = 1; h < tileObj.hitboxes.size(); h++)
                {
                    DrawAABB(*tileObj.GetSubAABB(h), MAGENTA);
                }

                Vector2 lineEnd = tileObj.transform.position;

                float halfW = tileObj.GetMainAABB()->width * 0.5f;
                float halfH = tileObj.GetMainAABB()->height * 0.5f;

                switch (tileObj.direction)
                {
                case Direction::UP: lineEnd.y -= halfH; break;
                case Direction::DOWN: lineEnd.y += halfH; break;
                case Direction::LEFT: lineEnd.x -= halfW; break;
                case Direction::RIGHT: lineEnd.x += halfW; break;
                default:break;
                }

                DrawLineEx(tileObj.transform.position, lineEnd, 1.0f ,GREEN);
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
