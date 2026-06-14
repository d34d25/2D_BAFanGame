#include "level.h"

#include <iostream>
#include <algorithm>

Level::Level() : player({0, 0})
{
}

Level::~Level()
{
    ClearTileMatrix();

    ClearPlatformList();

    UnloadAssets();
}

void Level::InitLevel(const char* levelPath, const char* roomPath ,float dt, int iterations)
{
    this->iterations = iterations;

    this->dt = dt;

    gravity = GRAVITY;

    player.gameObj.data.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    camera.zoom = CAMERA_ZOOM;
    
    camera.rotation = 0.0f;

    float step = 1.0f / (float)GRID_SIZE;

    camera.zoom = roundf(camera.zoom / step) * step;

    rangeLimits = GetTileRangeLimits();

    ClearTileMatrix();

    ClearPlatformList();

    LoadLevelData(levelPath, level, roomPath, rooms);

    platformList.reserve(800);

    enemyList.reserve(800);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = 0; i < COLS; i++)
        {
            for(int j = 0; j < ROWS; j++)
            {
                Tile* tile = &level[l][i][j];
                TileType type = tile->type;

                if(type == TileType::PLAYER_SPAWN)
                {
                    player.gameObj.transform.position = tile->gameObj.transform.position;
                    player.spawnPos = player.gameObj.transform.position;
                }

                if(IsTypeInvalid(type)) level[l][i][j].type = TileType::VOID;

                //platforms...

                bool isPlatform = type > TileType::PLATFORM_START && type < TileType::PLATFORM_END;

                if(isPlatform)
                {
                    platformList.emplace_back();

                    Platform& platform = platformList.back();

                    switch (type)
                    {
                    case TileType::HORIZONALT_MOVING_PLATFORM: platform.type = PlatformType::MOVING_HORIZONTAL; break;

                    case TileType::VERTICAL_MOVING_PLATFORM: platform.type = PlatformType::MOVING_VERTICAL; break;

                    case TileType::FALLING_PLATFORM: platform.type = PlatformType::FALLING; break;

                    case TileType::DISAPPEARING_PLATFORM: platform.type = PlatformType::DISAPPEARING; break;

                    case TileType::VERTICAL_MOVING_SPIKE: platform.type = PlatformType::MOVING_SPIKE_VERTICAL; break;

                    case TileType::HORIZONTAL_MOVING_SPIKE: platform.type = PlatformType::MOVING_SPIKE_HORIZONTAL; break;

                    case TileType::ROTATING_SPIKE_SINGLE: platform.type = PlatformType::ROTATING_SPIKE_SINGLE; break;

                    case TileType::ROTATING_SPIKE_DOUBLE: platform.type = PlatformType::ROTATING_SPIKE_DOUBLE; break;

                    default: break;
                    }

                    platform.InitPlatform(
                        tile->gameObj.transform,
                        tile->gameObj.data,
                        tile->gameObj.direction,
                        gravity,
                        tile->textureIndex,
                        tile->variantIndex
                    );
                }

                //enemies

                bool isEnemy = type > TileType::ENEMY_START && type < TileType::ENEMY_END;

                if(isEnemy)
                {
                    enemyList.emplace_back();

                    Enemy& enemy = enemyList.back();

                    switch (type)
                    {
                    case TileType::ENEMY_DUMMY: enemy.type = EnemyType::DUMMY;break; 

                    case TileType::ENEMY_YUUKA: enemy.type = EnemyType::YUUKA; break;
                    
                    default:
                    break;
                    }

                    enemy.InitEnemy(
                        tile->gameObj.transform.position,
                        tile->gameObj.data,
                        gravity
                    );

                    float nearPlayer = GRID_SIZE * 15.0f;

                    if(Vector2DistanceSqr(player.spawnPos, enemy.spawnPosition) <= nearPlayer * nearPlayer)
                        enemy.isActive = true;
                }

                //actual tiles
                if(IsNotRealTile(type))
                {
                    if(tile->type != TileType::VOID) tile->type = TileType::VOID;

                    continue;
                }

                int upLeftArray[3] = {l, i - 1, j - 1};
                int upArray[3] = {l, i, j - 1};
                int upRightArray[3] = {l, i + 1, j - 1};

                int rightArray[3] = {l, i + 1, j};
                int downRightArray[3] = {l, i +1, j + 1};
                int downArray[3] = {l, i, j + 1};

                int downLeftArray[3] = {l, i - 1, j + 1};
                int leftArray[3] = {l, i - 1, j};

                tile->neighborsTypes[0] = GetTileType(upLeftArray, level);
                tile->neighborsTypes[1] = GetTileType(upArray, level);
                tile->neighborsTypes[2] = GetTileType(upRightArray, level);

                tile->neighborsTypes[3] = GetTileType(rightArray, level);
                tile->neighborsTypes[4] = GetTileType(downRightArray, level);
                tile->neighborsTypes[5] = GetTileType(downArray, level);

                tile->neighborsTypes[6] = GetTileType(downLeftArray, level);
                tile->neighborsTypes[7] = GetTileType(leftArray, level);

                tile->gameObj.transform.scale = TILE_SCALE;

                //decorational tiles don't need a physical body
                if(level[l][i][j].type == TileType::DECO) continue;

                bool upLeft = IsTileNotEqual(upLeftArray, level, TileType::SOLID);
                bool up = IsTileNotEqual(upArray, level, TileType::SOLID);
                bool upRight = IsTileNotEqual(upRightArray, level, TileType::SOLID);

                bool right = IsTileNotEqual(rightArray, level, TileType::SOLID);
                bool downRight = IsTileNotEqual(downRightArray, level, TileType::SOLID);
                bool down = IsTileNotEqual(downArray, level, TileType::SOLID);

                bool downLeft = IsTileNotEqual(downLeftArray, level, TileType::SOLID);
                bool left = IsTileNotEqual(leftArray, level, TileType::SOLID);

                bool isEdge = upLeft || up || upRight ||
                right || downRight || down ||
                downLeft || left;

                if(!isEdge) continue;

                tile->gameObj.hasBody = true;

                tile->gameObj.body = {};

                tile->gameObj.hitboxes.push_back(Hitbox{{0,0}, {GRID_SIZE, GRID_SIZE}});

                tile->gameObj.UpdateHitboxes();

                float treadmillVel = 200.0f;

                switch (type)
                {
                case TileType::SOLID:
                {
                    tile->gameObj.canEntityCollidePhysically = true;
                    tile->gameObj.canPlatformCollidePhysically = true;

                    tile->isJumpTrigger = true;
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

                    tile->isJumpTrigger = true;

                    tile->gameObj.body.velocity.x = treadmillVel;
                }
                break;

                case TileType::TREADMILL_LEFT:
                {
                    tile->gameObj.canEntityCollidePhysically = true;
                    tile->gameObj.canPlatformCollidePhysically = true;

                    tile->isJumpTrigger = true;

                    tile->gameObj.body.velocity.x = -treadmillVel;
                }
                break;

                case TileType::ONE_WAY:
                {
                    tile->gameObj.canEntityCollidePhysically = true;
                    tile->gameObj.canPlatformCollidePhysically = true;

                    if(IsOneWayUpDown(*tile)) tile->isJumpTrigger = true;
                }
                break;

                case TileType::LADDER:
                {
                    tile->gameObj.canPlatformCollidePhysically = true;
                }
                break;

                default:
                {
                    tile->gameObj.canEntityCollidePhysically = false;
                    tile->gameObj.canPlatformCollidePhysically = false;
                    tile->isJumpTrigger = false;
                }
                break;
                }

                if(IsTileSpike(tile->type))
                {
                    tile->gameObj.canEntityCollidePhysically = false;
                    tile->gameObj.canPlatformCollidePhysically = false;

                    SpriteRenderData* spikeRenderData = GetTileActiveRenderData(TileType::SPIKE);

                    auto AddSpikeHitbox = [&](float widthFactor, float heightFactor, float offsetX, float offsetY)
                    {
                        if(tile->gameObj.direction == Direction::LEFT || tile->gameObj.direction == Direction::RIGHT)
                        {
                            std::swap(widthFactor, heightFactor);
                        }

                        Vector2 size = {tile->gameObj.GetMainAABB().width * widthFactor, tile->gameObj.GetMainAABB().height * heightFactor};

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

                    case TileType::SPIKE_BALL:
                    {
                        AddSpikeHitbox(0.8f,0.8f,0.0f,0.0f);
                    }
                    break;

                    default: break;
                    }
                }
            }
        }
    }

    std::stable_sort(platformList.begin(), platformList.end(), [](const Platform& a, const Platform& b)
    {
        bool isASpike = IsPlatformSpike(a.type);
        bool isBSpike = IsPlatformSpike(b.type);

        if(isASpike != isBSpike) return !isASpike;

        return false;
    });

    enemyBuckets.resize(rooms.size());

    for(int i = 0; i < enemyList.size(); i++)
    {
        for(int r = 0; r < rooms.size(); r++)
        {    
            if(CheckCollisionPointRec(enemyList[i].spawnPosition, rooms[r].aabb))
            {
                enemyList[i].roomSize = rooms[r].aabb;

                enemyBuckets[r].push_back(std::move(enemyList[i]));
                break;
            }
        }
    }

    enemyList.clear();
    enemyList.shrink_to_fit();

    platformBuckets.resize(rooms.size());

    for(int i = 0; i < platformList.size(); i++)
    {
        for(int r = 0; r < rooms.size(); r++)
        {
            if(CheckCollisionPointRec(platformList[i].ogPosition, rooms[r].aabb))
            {
                platformBuckets[r].push_back(std::move(platformList[i]));
                break;
            }
        }
    }

    platformList.clear();
    platformList.shrink_to_fit();

    player.platformCache_update.reserve(800);
    player.platformCache_physics.reserve(60);
    player.platformCache_rendering.reserve(600);

    player.enemyCache.reserve(800);
}

void Level::UpdateLevel()
{
    lowFrequencyCounter++;

    if(lowFrequencyCounter % 4 == 0)
    {
        LowFrequencyUpdate();
        lowFrequencyCounter = 0;
    }

    MediumFrequencyDiscreteUpdate();

    for(int iteraion = 0; iteraion < iterations; iteraion++)
    {
        HighFrequencyDiscreteUpdate();
    }

    CCD_Update();

    if(player.canMove) player.Shoot(dt);

    UpdateCamera(player.gameObj.transform.position, {0, -100});

    if(IsKeyPressed(KEY_R)) ResetLevel();

    if(IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) camera.zoom = CAMERA_ZOOM;
}

void Level::ResetRoom()
{
    if(previousRoomIndex > -1 && previousRoomIndex < enemyBuckets.size())
    {
        for(Enemy& enemy : enemyBuckets[previousRoomIndex])
        {
            enemy.isActive = false;
            enemy.Respawn();
        }

        for(Platform& platform : platformBuckets[previousRoomIndex])
        {
            platform.Respawn();
        }
    }

    if(currentRoomIndex > -1 && currentRoomIndex < enemyBuckets.size())
    {
        for(Enemy& enemy : enemyBuckets[currentRoomIndex])
        {
            enemy.isActive = true;
            enemy.Respawn();
        }

        for(Platform& platform : platformBuckets[currentRoomIndex])
        {
            platform.Respawn();
        }
    }
}

void Level::LowFrequencyUpdate()
{
    player.platformCache_update.clear();

    player.enemyCache.clear();
    
    float playerRadius = player.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER;
    float platformUpdateRadius = GRID_SIZE * 25.0f;

    float enemySpawnRadius = GRID_SIZE * 15.0f;
    float enemyDespawnRadius = GRID_SIZE * 17.0f;

    if(currentRoomIndex > -1 && currentRoomIndex < enemyBuckets.size())
    {
        std::vector<Enemy>& activeEnemyBucket = enemyBuckets[currentRoomIndex];

        for(int e = 0; e < activeEnemyBucket.size(); e++)
        {
            Enemy& enemy = activeEnemyBucket[e];

            float despawnDistanceSqr = Vector2DistanceSqr(camera.target, enemy.gameObj.transform.position);
            float spawnDistanceSqr = Vector2DistanceSqr(camera.target, enemy.spawnPosition);

            if(despawnDistanceSqr > enemyDespawnRadius * enemyDespawnRadius)
            {
                enemy.isActive = false;
            }
            else if(!enemy.isActive)
            {
                if(spawnDistanceSqr <= enemyDespawnRadius * enemyDespawnRadius &&
                spawnDistanceSqr >= enemySpawnRadius * enemySpawnRadius)
                {
                    enemy.Respawn();
                    enemy.isActive = true;
                }
            }

            if(enemy.isActive)
            {
                player.enemyCache.push_back(&enemy);
            }
        }
    }

    if(currentRoomIndex > -1 && currentRoomIndex < platformBuckets.size())
    {
        std::vector<Platform>& activePLatformBucket = platformBuckets[currentRoomIndex];

        for(int p = 0; p < activePLatformBucket.size(); p++)
        {
            Platform& platform = activePLatformBucket[p];

            if(!platform.gameObj.hasBody) continue;

            if(platform.IsInactive())
            {
                platform.UpdateInactive(dt, 1);
                continue;
            }
            else if(platform.type == PlatformType::FALLING && platform.respawnTimer >= 0.0f && platform.updateRequired)
            {
                platform.Update(dt, 1);
            }

            if(CheckCollisionCircles(
                player.gameObj.transform.position,
                playerRadius,
                platform.gameObj.transform.position,
                platformUpdateRadius
            ))
            {
                player.platformCache_update.push_back(&platform);
            }
        }
    }
}

void Level::MediumFrequencyDiscreteUpdate()
{
    player.platformCache_physics.clear();
    player.platformCache_rendering.clear();

    player.enemyCache_physics.clear();

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    float renderRadius = GRID_SIZE * 20.0f;

    for(int p = 0; p < player.platformCache_update.size(); p++)
    {
        Platform* platform = player.platformCache_update[p];

        if(!platform) continue;

        bool isMovingPlatform = platform->type > PlatformType::MOVING_START && platform->type < PlatformType::MOVING_END;

        bool isRespawnPlatform = platform->type == PlatformType::FALLING || platform->type == PlatformType::DISAPPEARING;

        if(CheckCollisionCircles(
            player.gameObj.transform.position,
            playerRadius,
            platform->gameObj.transform.position,
            renderRadius
        ))
        {
            player.platformCache_rendering.push_back(platform);
        }

        if(platform->updateRequired) platform->Update(dt, 1);

        if(isMovingPlatform)
        {
            TileRange platformRange = CalculateTileRange(
            platform->gameObj.transform.position.x,
            platform->gameObj.transform.position.y,
            collisionTileCheckRange,
            rangeLimits.minX, rangeLimits.minY,
            rangeLimits.maxX, rangeLimits.maxY
            );

            for(int l = 0; l < LAYERS; l++)
            {
                for(int i = platformRange.startX; i <= platformRange.endX; i++)
                {
                    for(int j = platformRange.startY; j <= platformRange.endY; j++)
                    {
                        GameObject& objTile = level[l][i][j].gameObj;

                        if(!objTile.canPlatformCollidePhysically) continue;

                        if(objTile.hitboxes.empty()) continue;

                        if(CheckCollisionCircles(
                            platform->gameObj.transform.position, 
                            platform->gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
                            objTile.transform.position,
                            objTile.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                        ))
                        {
                            if(CheckCollisionRecs(platform->gameObj.GetMainAABB(), objTile.GetMainAABB()))
                            {
                                SolveCollisions_Platform(&platform->gameObj, &objTile, (platform->type > PlatformType::MOVING_X && platform->type < PlatformType::MOVING_Y));
                            }
                        }
                    }
                }
            }
        }

        if(CheckCollisionCircles(
            player.gameObj.transform.position,
            playerRadius,
            platform->gameObj.transform.position, 
            GRID_SIZE * 5.0f
        ))
        {
            player.platformCache_physics.push_back(platform);
        }
    }

    //player vs enemy cache
    for(int e = 0; e < player.enemyCache.size(); e++)
    {
        Enemy* enemy = player.enemyCache[e];

        if(!enemy) continue;

        if(CheckCollisionCircles(
            player.gameObj.transform.position,
            playerRadius,
            enemy->gameObj.transform.position,
            GRID_SIZE * 3.0f
        ))
        {
            player.enemyCache_physics.push_back(enemy);
        }

        if(lowFrequencyCounter % 2 == 0)
        {
            enemy->UpdateAI(dt, player.gameObj.transform.position, &player.gameObj.body.velocity, player.isGrounded, &player.canMove);

            enemy->ResetFlagsAI();
        }

        if(enemy->shooting) enemy->Shoot(dt);
    }
}

void Level::HighFrequencyDiscreteUpdate()
{
    Vector2 playerFuturePos = {
        player.gameObj.transform.position.x + player.gameObj.body.GetFinalVelocity().x * (dt / iterations),
        player.gameObj.transform.position.y + player.gameObj.body.GetFinalVelocity().y * (dt / iterations)
    };

    int roomIndex = -1;

    for(int r = 0; r < rooms.size(); r++)
    {
        if(CheckCollisionPointRec(playerFuturePos, rooms[r].aabb))
        {
            roomIndex = r;
            break;
        }
    }

    if(roomIndex != currentRoomIndex)
    {
        previousRoomIndex = currentRoomIndex;
        currentRoomIndex = roomIndex;

        rangeLimits = GetTileRangeLimits();

        ResetRoom();
    }

    isGravityUp = gravity < 0;

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
        collisionTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
    );

    player.Update(dt, iterations);

    player.ResetFalgs();

    //player X pass

    player.gameObj.UpdatePositionX(dt, iterations);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                if(!tile.gameObj.canEntityCollidePhysically) continue;

                if(!tile.gameObj.hasBody || tile.gameObj.hitboxes.empty()) continue;

                if(!CheckCollisionCircles(
                    player.gameObj.transform.position,
                    playerRadius,
                    tile.gameObj.transform.position,
                    tile.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                if(CheckCollisionRecs(player.gameObj.GetMainAABB(), tile.gameObj.GetMainAABB()))
                {
                    if(!IsTileOneWay(tile))
                    {
                        SolveCollisions(
                            &player.gameObj, &tile.gameObj, 
                            true, isGravityUp, 
                            tile.type == TileType::TRAMPOLINE,
                            false
                        );
                    }
                    else if(IsOneWayRightLeft(tile))
                    {
                        SolveCollisionsOneWayLeftRight(
                            &player.gameObj, &tile.gameObj,
                            tile.gameObj.direction == Direction::RIGHT
                        );
                    }
                }
            }
        }
    }

    //player Y pass

    player.gameObj.UpdatePositionY(dt, iterations);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                GameObject& objTile = tile.gameObj;

                if(!objTile.hasBody || objTile.hitboxes.empty()) continue;

                if(!CheckCollisionCircles(
                    player.gameObj.transform.position,
                    playerRadius,
                    objTile.transform.position,
                    objTile.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                bool isTileJumpTrigger = tile.isJumpTrigger;

                if(objTile.canEntityCollidePhysically)
                {
                    if(CheckCollisionRecs(player.gameObj.GetMainAABB(), objTile.GetMainAABB()))
                    {
                        if(!IsTileOneWay(tile))
                        {
                            SolveCollisions(
                                &player.gameObj, &objTile, 
                                false, isGravityUp, 
                                tile.type == TileType::TRAMPOLINE,
                                false
                            );
                        }
                        else if(IsOneWayUpDown(tile))
                        {
                            SolveCollisionsOneWayUpDown
                            (
                                &player.gameObj, &objTile,
                                tile.gameObj.direction == Direction::UP,
                                isGravityUp,
                                false
                            );
                        }
                    }

                    if(CheckCollisionRecs(player.GetCeilingDetector(), objTile.GetMainAABB()))
                    {
                        if(!IsOneWayUpDown(tile)) player.hitCeiling = true;
                        else if(IsOneWayUpDown(tile))
                        {
                            if(tile.gameObj.direction == Direction::UP &&
                                IsAbove(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
                            )
                            {
                                player.hitCeiling = true;
                            }
                            else if(tile.gameObj.direction == Direction::DOWN &&
                                IsBelow(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
                            )
                            {
                                player.hitCeiling = true;
                            }
                        }
                    }
                }

                //tile triggers and non solid tiles

                //if bugs with tirggers happen move this back into its own loop
                
                if(IsOneWayRightLeft(tile)) continue;

                if(CheckCollisionRecs(player.gameObj.GetMainAABB(), objTile.GetMainAABB()))
                {
                    switch (tile.type)
                    {
                    case TileType::GRAVITY_CHANGER: player.wasTouchingGravityChanger = true; break;
                    case TileType::WIND:
                    {
                        if(!player.windApplied)
                        {
                            bool isEdgeUp = tile.gameObj.direction == Direction::UP && 
                            (tile.GetNeighborType(NeighborDirection::UP) == TileType::VOID ||
                            tile.GetNeighborType(NeighborDirection::UP) == TileType::DECO);

                            bool isEdgeDown = tile.gameObj.direction == Direction::DOWN &&
                            (tile.GetNeighborType(NeighborDirection::DOWN) == TileType::VOID ||
                            tile.GetNeighborType(NeighborDirection::DOWN) == TileType::DECO);

                            ApplyWind(
                                &player.gameObj,
                                &objTile,
                                tile.gameObj.direction,
                                isEdgeUp,
                                isEdgeDown,
                                isGravityUp
                            );

                            player.windApplied = true;
                        }
                    }
                    break;

                    case TileType::WATER:
                    {
                        if(!player.inWater)
                        {
                            ApplyWaterPhysics(&player.gameObj, isGravityUp);

                            player.inWater = true;
                        }
                    }
                    break;

                    case TileType::LADDER:
                    {
                        if(!player.inLadder)
                        {
                            player.inLadder = true;

                            player.laddedSnapPosX = tile.gameObj.transform.position.x;
                        }

                        bool isEdgeUp = tile.GetNeighborType(NeighborDirection::UP) == TileType::VOID ||
                        tile.GetNeighborType(NeighborDirection::UP) == TileType::DECO;

                        bool isEdgeDown = tile.GetNeighborType(NeighborDirection::DOWN) == TileType::VOID ||
                        tile.GetNeighborType(NeighborDirection::DOWN) == TileType::DECO;

                        if((!isGravityUp && isEdgeUp) || (isGravityUp && isEdgeDown))
                        {
                            if((IsAbove(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 1.0f) && !isGravityUp) || 
                            (IsBelow(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 1.0f) && isGravityUp))
                            {
                                if(!player.IsPressingDown())
                                {
                                    SolveCollisionsOneWayUpDown(&player.gameObj, &objTile, true, isGravityUp, true);

                                    player.inLadder = false;
                                    isTileJumpTrigger = true;
                                }
                            }
                            else 
                            {
                                isTileJumpTrigger = false;
                            }
                        }
                    }
                    break;
                    
                    default:
                        break;
                    }

                    if(IsTileSpike(tile.type))
                    {
                        for(int h = 0; h < objTile.hitboxes.size(); h++)
                        {
                            if(CheckCollisionRecs(player.gameObj.GetMainAABB(), objTile.GetSubAABB(h)))
                            {
                                player.wasTouchingSpike = true;
                                break;
                            }
                        }
                    }
                }

                if(!isTileJumpTrigger) continue;

                if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
                {
                    if(CheckCollisionRecs(player.GetTreadmillDetector(), objTile.GetMainAABB()) && player.IsFalling())
                        player.gameObj.body.altVelocity = objTile.body.velocity;
                }

                if(CheckCollisionRecs(player.GetJumpDetector(), objTile.GetMainAABB()) && player.IsFalling())
                {
                    if(!IsOneWayUpDown(tile)) player.wasGrounded = true;
                    else if(IsOneWayUpDown(tile))
                    {
                        if(tile.gameObj.direction == Direction::UP &&
                            IsAbove(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
                        )
                        {
                            player.wasGrounded = true;
                        }
                        else if(tile.gameObj.direction == Direction::DOWN &&
                            IsBelow(player.gameObj.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
                        )
                        {
                            player.wasGrounded = true;
                        }
                    }
                }
            }
        }
    }

    //player vs platforms

    for(int i = 0; i < player.platformCache_physics.size(); i++)
    {
        Platform* platform = player.platformCache_physics[i];

        for(int h = 1; h < platform->gameObj.hitboxes.size(); h++)
        {
            if(CheckCollisionRecs(player.gameObj.GetMainAABB(), platform->gameObj.GetSubAABB(h)))
            {
                if(IsPlatformSpike(platform->type))
                {
                    player.wasTouchingSpike = true;
                }
            }
        }

        if(!CheckCollisionCircles(
            player.gameObj.transform.position,
            player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
            platform->gameObj.transform.position,
            platform->gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
        )) continue;
        
        if(!IsPlatformSpike(platform->type))
        {
            if(CheckCollisionRecs(player.gameObj.GetMainAABB(), platform->gameObj.GetMainAABB()))
            {
                SolveCollisionsOneWayUpDown(
                    &player.gameObj, &platform->gameObj,
                    true, isGravityUp, true
                );
            }
        }

        if(CheckCollisionRecs(player.GetJumpDetector(), platform->gameObj.GetMainAABB()) && player.IsFalling())
        {
            if(!isGravityUp && IsAbove(player.gameObj.GetMainAABB(), platform->gameObj.GetMainAABB(), 0.0f) || 
            (isGravityUp && IsBelow(player.gameObj.GetMainAABB(), platform->gameObj.GetMainAABB(), 0.0f)))
            {
                platform->updateRequired = true;
                player.wasGrounded = true;
            }
        }
    }
   
    //player vs enemies

    //enemies vs tiles

    for(int e = 0; e < player.enemyCache.size(); e++)
    {
        Enemy* enemy = player.enemyCache[e];

        float enemyRadius = enemy->gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

        TileRange enemyTileRange = CalculateTileRange(
            enemy->gameObj.transform.position.x,
            enemy->gameObj.transform.position.y,
            collisionTileCheckRange,
            rangeLimits.minX, rangeLimits.minY,
            rangeLimits.maxX, rangeLimits.maxY
        );

        enemy->Update(dt, iterations);

        enemy->ResetFlags();

        //enemy x pass
        enemy->gameObj.UpdatePositionX(dt, iterations);

        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = enemyTileRange.startX; i <= enemyTileRange.endX; i++)
            {
                for(int j = enemyTileRange.startY; j <= enemyTileRange.endY; j++)
                {
                    Tile& tile = level[l][i][j];

                    if(!tile.gameObj.canEntityCollidePhysically) continue;

                    if(!CanEnemyCollideWithTile(tile.type)) continue; 

                    if(!tile.gameObj.hasBody || tile.gameObj.hitboxes.empty()) continue;

                    if(!CheckCollisionCircles(
                        enemy->gameObj.transform.position,
                        enemyRadius,
                        tile.gameObj.transform.position,
                        tile.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                    )) continue;

                    if(CheckCollisionRecs(
                        enemy->gameObj.GetMainAABB(),
                        tile.gameObj.GetMainAABB()
                    ))
                    {
                        enemy->isTouchingWall = true;

                        if(!IsTileOneWay(tile))
                        {
                            SolveCollisions(
                                &enemy->gameObj, &tile.gameObj, 
                                true, isGravityUp, 
                                tile.type == TileType::TRAMPOLINE,
                                false
                            );
                        }
                        else if(IsOneWayRightLeft(tile))
                        {
                            SolveCollisionsOneWayLeftRight(
                                &enemy->gameObj, &tile.gameObj,
                                tile.gameObj.direction == Direction::RIGHT
                            );
                        }
                    }
                }
            }
        }

        //enemy y pass
        enemy->gameObj.UpdatePositionY(dt, iterations);

        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = enemyTileRange.startX; i <= enemyTileRange.endX; i++)
            {
                for(int j = enemyTileRange.startY; j <= enemyTileRange.endY; j++)
                {
                    Tile& tile = level[l][i][j];

                    if(!CanEnemyCollideWithTile(tile.type)) continue; 

                    if(!tile.gameObj.hasBody || tile.gameObj.hitboxes.empty()) continue;

                    if(!CheckCollisionCircles(
                        enemy->gameObj.transform.position,
                        enemyRadius,
                        tile.gameObj.transform.position,
                        tile.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                    )) continue;

                    if(tile.gameObj.canEntityCollidePhysically)
                    {
                        if(CheckCollisionRecs(
                        enemy->gameObj.GetMainAABB(),
                        tile.gameObj.GetMainAABB()
                        ))
                        {
                            if(!IsTileOneWay(tile))
                            {
                                SolveCollisions(
                                    &enemy->gameObj, &tile.gameObj, 
                                    false, false, 
                                    tile.type == TileType::TRAMPOLINE,
                                    false
                                );
                            }
                            else if(IsOneWayUpDown(tile))
                            {
                                SolveCollisionsOneWayUpDown
                                (
                                    &enemy->gameObj, &tile.gameObj,
                                    tile.gameObj.direction == Direction::UP,
                                    false,
                                    false
                                );
                            }
                        }

                        if(CheckCollisionRecs(enemy->GetCeilingDetector(), tile.gameObj.GetMainAABB()))
                        {
                            if(!IsOneWayUpDown(tile)) enemy->hitCeiling = true;
                            else if(IsOneWayUpDown(tile))
                            {
                                if(tile.gameObj.direction == Direction::UP &&
                                    IsAbove(enemy->gameObj.GetMainAABB(), tile.gameObj.GetMainAABB(), 0.0f)
                                )
                                {
                                    enemy->hitCeiling = true;
                                }
                                else if(tile.gameObj.direction == Direction::DOWN &&
                                    IsBelow(enemy->gameObj.GetMainAABB(), tile.gameObj.GetMainAABB(), 0.0f)
                                )
                                {
                                    enemy->hitCeiling = true;
                                }
                            }
                        }
                    }
                    
                    if(IsOneWayRightLeft(tile)) continue;
                    
                    if(CheckCollisionRecs(enemy->gameObj.GetMainAABB(),tile.gameObj.GetMainAABB()))
                    {
                        switch (tile.type)
                        {
                        case TileType::WATER:
                        {
                            if(!enemy->inWater)
                            {
                                ApplyWaterPhysics(&enemy->gameObj, false);
                                
                                enemy->inWater = true;
                            }
                        }
                        break;
                        
                        default:
                            break;
                        }
                    }

                    if(!tile.isJumpTrigger) continue;

                    if(CheckCollisionRecs(enemy->GetJumpDetector(), tile.gameObj.GetMainAABB()))
                    {
                        if(!IsOneWayUpDown(tile)) enemy->wasGrounded = true;
                        else if(IsOneWayUpDown(tile))
                        {
                            if(tile.gameObj.direction == Direction::UP &&
                                IsAbove(enemy->gameObj.GetMainAABB(), tile.gameObj.GetMainAABB(), 0.0f)
                            )
                            {
                                enemy->wasGrounded = true;
                            }
                            else if(tile.gameObj.direction == Direction::DOWN &&
                                IsBelow(enemy->gameObj.GetMainAABB(), tile.gameObj.GetMainAABB(), 0.0f)
                            )
                            {
                                enemy->wasGrounded = true;
                            }
                        }
                    }
                }
            }
        }

        enemy->UpdateFlags();
    }

    if(!player.isTouchingGravityChanger && player.wasTouchingGravityChanger)
    {
        player.isGrounded = false;
        player.isJumping = false;

        gravity *= -1;
        isGravityUp = gravity < 0;

        player.gravity = gravity;
        player.gameObj.data.flipY = isGravityUp;

        if(currentRoomIndex > -1 && currentRoomIndex < platformBuckets.size())
        {
            std::vector<Platform>& activePLatformBucket = platformBuckets[currentRoomIndex];

            for(int i = 0; i < activePLatformBucket.size(); i++)
            {
                Platform& platform = activePLatformBucket[i];

                if(!(platform.type == PlatformType::FALLING) || platform.updateRequired) continue;

                platform.gravity = gravity;
            }
        }
    }

    if(!player.isTouchingSpike && player.wasTouchingSpike)
    {
        ResetLevel();
    }

    //booleans update

    player.UpdateFlags();
}

void Level::CCD_Update()
{
    player.bulletpool.get()->UpdateBullets(dt);

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* bullet = player.bulletpool->activeBullets[i];

        if(!bullet) continue;
        
        if(currentRoomIndex > -1 && currentRoomIndex < rooms.size())
        {
            Rectangle& currentRoom = rooms[currentRoomIndex].aabb;

            if(!CheckCollisionPointRec(bullet->posititon, currentRoom)) bullet->didHit = true;
        }

        for(int e = 0; e < player.enemyCache.size(); e++)
        {
            Enemy* enemy = player.enemyCache[e];

            if(!enemy) continue;

            CCD_CollisionResult result = CheckCollisionsBulletVsEntity_CCD(bullet, &enemy->gameObj, dt);

            if(result.collision)
            {
                if(!player.bulletpool.get()->pierces) bullet->didHit =  true;
            }
        }

        if(player.bulletpool->explodes)
        {
            TileRange bulletRange = CalculateTileRange(
                bullet->posititon.x, bullet->posititon.y,
                2.0f, rangeLimits.minX, rangeLimits.minY,
                rangeLimits.maxX, rangeLimits.maxY 
            );

            for(int l = 0; l < LAYERS; l++)
            {
                for(int i = bulletRange.startX; i <= bulletRange.endX; i++)
                {
                    for(int j = bulletRange.startY; j <= bulletRange.endY; j++)
                    {
                        Tile& tile = level[l][i][j];

                        if(tile.gameObj.canEntityCollidePhysically)
                        {
                            if(CheckCollisionCircleRec(
                                bullet->posititon, bullet->radius,
                                tile.gameObj.GetMainAABB()
                            ))
                            {
                                bullet->didHit = true;
                            }
                        }
                    }
                }
            }
        }
        
    }

    for(int i = 0; i < player.enemyCache.size(); i++)
    {
        Enemy* enemy = player.enemyCache[i];

        if(!enemy) continue;

        enemy->bulletpool.get()->UpdateBullets(dt);
    }
}

void Level::ResetLevel()
{
    if(gravity < 0) gravity *= -1;

    bool gravityUp = gravity < 0;

    player.gravity = gravity;
    player.gameObj.data.flipY = gravityUp;

    for(int i = 0; i < platformBuckets.size(); i++)
    {
        for(int j = 0; j < platformBuckets[i].size(); j++)
        {
            Platform& platform = platformBuckets[i][j];

            platform.Respawn();

            if(platform.type == PlatformType::FALLING)
            {
                platform.gravity = gravity;
            }
        }
    }

    for(int i = 0; i < enemyBuckets.size(); i++)
    {
        for(int j = 0; j < enemyBuckets[i].size(); j++)
        {
            Enemy& enemy = enemyBuckets[i][j];

            enemy.Respawn();
            enemy.isActive = false;
        }
    }

    player.Respawn();
}

void Level::DrawLevel()
{
    double currentTime = GetTime();

    BeginTextureMode(canvas);

    ClearBackground(LIGHTGRAY);

    BeginMode2D(camera);
    
    TileRange playerTileRange = CalculateTileRange(
        camera.target.x,
        camera.target.y,
        renderTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
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
                            tileRenderData->animationSpeed,
                            currentTime
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

                    if(IsColorOf(color, BLANK) || tile.type == TileType::PLATFORM_STOP) continue;

                    if(!tile.gameObj.hitboxes.empty()) DrawRectangleRec(tile.gameObj.GetMainAABB(), color);
                    else DrawRectangle(i * GRID_SIZE, j * GRID_SIZE, GRID_SIZE, GRID_SIZE, color);
                }
            }
        }
    }

    for(int i = 0; i < player.platformCache_rendering.size(); i++)
    {
        Platform* platform = player.platformCache_rendering[i];

        SpriteRenderData* platformRenderData =  GetPlatformActiveRenderData(platform->type, platform->variantIndex);

        if(platformRenderData)
        {
            int frameToDraw = platform->textureIndex;

            if(platformRenderData->spacing != 1)
            {
                frameToDraw = GetCurrentFrame(
                    platformRenderData->animationFrames,
                    platform->textureIndex,
                    platformRenderData->spacing,
                    platformRenderData->animationSpeed,
                    currentTime
                );
            }

            if(frameToDraw >= 0 && frameToDraw < (int)platformRenderData->animationFrames.size())
            {
                DrawSprite(platform->gameObj, platformRenderData, frameToDraw);
            }
        }
        else
        {
            Color platformColor = DECO;

            if(platform->type == PlatformType::MOVING_VERTICAL) platformColor = VERTICAL_MOVING_PLATFORM;
            else if(platform->type == PlatformType::MOVING_HORIZONTAL) platformColor = HORIZONTAL_MOVING_PLATFORM;
            else if(platform->type == PlatformType::FALLING) platformColor = FALLING_PLATFORM;
            else if(platform->type == PlatformType::DISAPPEARING) platformColor = DISAPPEARING_PLATFORM;

            if(!IsPlatformSpike(platform->type))
                DrawRectangleRec(platform->gameObj.GetMainAABB(), platformColor);
            else
            {
                for(int h = 1; h < platform->gameObj.hitboxes.size(); h++)
                {
                    DrawRectangleRec(platform->gameObj.GetSubAABB(h), SPIKE);
                }
            }
        }
    }

    for(int i = 0; i < player.enemyCache.size(); i++)
    {
        Enemy* enemy = player.enemyCache[i];

        DrawRectangleRec(enemy->gameObj.GetMainAABB(), enemy->testColor);
    }

    DrawSprite(
        player.gameObj.transform,
        &player.characterRenderData,
        player.gameObj.data,
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

            DrawExplosion(e->position.x, e->position.y, e->radius, e->renderData, 0, TILE_SCALE);
        }
    }

    for(int e = 0; e < player.enemyCache.size(); e++)
    {
        Enemy* enemy = player.enemyCache[e];

        if(!enemy) continue;

        for(int b = 0; b < enemy->bulletpool->activeBullets.size(); b++)
        {
            Bullet* bullet = enemy->bulletpool->activeBullets[b];

            if(!bullet) continue;

            DrawBullet(bullet->posititon.x, bullet->posititon.y, bullet->radius, bullet->mainColor, bullet->backColor);
        }
    }

    DrawSprite(
        player.gameObj.transform,
        &player.weaponRenderData,
        player.gameObj.data,
        player.currentFrame
    );

    //DebugDrawing();

    EndMode2D();

    EndTextureMode();

    ClearBackground(BLACK);

    float scale = fminf((float)GetScreenWidth() / screenWidth, 
    (float)GetScreenHeight() / screenHeight);

    Rectangle sourceRec = {0,0, (float)canvas.texture.width, (float)-canvas.texture.height};

    Rectangle destRec = {
        ((float)GetScreenWidth() - ((float)screenWidth * scale)) * 0.5f,
        ((float)GetScreenHeight() - ((float)screenHeight * scale)) * 0.5f,
        (float)screenWidth * scale,
        (float)screenHeight * scale
    };

    DrawTexturePro(
        canvas.texture,
        sourceRec,
        destRec,
        {0,0},
        0.0f,
        WHITE
    );

    //DebugTextDrawing();
}

void Level::DebugDrawing()
{
    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < player.platformCache_rendering.size(); i++)
    {
        Platform* platform = player.platformCache_rendering[i];

        DrawLine(
            player.gameObj.transform.position.x, player.gameObj.transform.position.y, 
            platform->gameObj.transform.position.x, platform->gameObj.transform.position.y, 
            RED
        );

        DrawAABB(platform->gameObj.GetMainAABB(), RED);

        for(int h = 1; h < platform->gameObj.hitboxes.size(); h++)
        {
            DrawAABB(platform->gameObj.GetSubAABB(h), MAGENTA);
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

                Color mainAABBColor = RED;

                if(level[l][i][j].type == TileType::PLATFORM_STOP) mainAABBColor = BLUE;

                DrawAABB(tileObj.GetMainAABB(), mainAABBColor, 2.0f);

                for(int h = 1; h < tileObj.hitboxes.size(); h++)
                {
                    DrawAABB(tileObj.GetSubAABB(h), MAGENTA, 1.25f);
                }

                Vector2 lineEnd = tileObj.transform.position;

                float halfW = tileObj.GetMainAABB().width * 0.5f;
                float halfH = tileObj.GetMainAABB().height * 0.5f;

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

    for(int i = 0; i < player.enemyCache.size(); i++)
    {
        Enemy* enemy = player.enemyCache[i];

        DrawAABB(enemy->gameObj.GetMainAABB(), RED);

        for(int h = 1; h < enemy->gameObj.hitboxes.size(); h++)
        {
            DrawAABB(enemy->gameObj.GetSubAABB(h), MAGENTA);
        }

        DrawAABB(enemy->GetJumpDetector(), GOLD);
        DrawAABB(enemy->GetCeilingDetector(), YELLOW);
    }
    
    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawCircleLines(b->posititon.x, b->posititon.y, b->radius, RED);

        DrawLine(b->posititon.x, b->posititon.y, b->posititon.x + b->velocity.x, b->posititon.y + b->velocity.y, BLUE);
    }

    for(int i = 0; i < rooms.size(); i++)
    {
        Room& room = rooms[i];

        Color roomColor = PURPLE;

        roomColor.a = 150;

        DrawAABB(room.aabb, roomColor, 3.0f);

        if(CheckCollisionRecs(player.gameObj.GetMainAABB(), room.aabb))
        {
            DrawAABB(room.aabb, RED, 3.0f);
        }
    }

    Color gridColor = GRAY;
    gridColor.a = (int)(255 * 0.5f);

    for(int i = 0; i <= ROWS * GRID_SIZE; i+= GRID_SIZE)
    {
        DrawLine(i, 0, i, ROWS * GRID_SIZE, gridColor);
    }

    for(int j = 0; j <= COLS * GRID_SIZE; j+= GRID_SIZE)
    {
        DrawLine(0, j, COLS * GRID_SIZE, j, gridColor);
    }

    DrawAABB(player.gameObj.GetMainAABB(), ORANGE);

    DrawAABB(player.GetJumpDetector(), GREEN);

    DrawAABB(player.GetCeilingDetector(), DARKGREEN);
}

void Level::DebugTextDrawing()
{
    DrawText(TextFormat("Iterations: %i", iterations), 10, 60, 20, SKYBLUE);

    DrawText(TextFormat("Player X speed: %.4f", player.gameObj.body.velocity.x), 10, 100, 20, GRAY);
    DrawText(TextFormat("Player Y speed: %.4f", player.gameObj.body.velocity.y), 10, 120, 20, GRAY);

    DrawText(TextFormat("Player alt X speed: %.4f", player.gameObj.body.altVelocity.x), 10, 160, 20, GRAY);
    DrawText(TextFormat("Player alt Y speed: %.4f", player.gameObj.body.altVelocity.y), 10, 180, 20, GRAY);

    DrawText(TextFormat("Player final X speed: %.4f", player.gameObj.body.GetFinalVelocity().x), 10, 220, 20, GRAY);
    DrawText(TextFormat("Player final Y speed: %.4f", player.gameObj.body.GetFinalVelocity().y), 10, 240, 20, GRAY);
}