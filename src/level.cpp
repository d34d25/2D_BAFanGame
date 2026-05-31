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

void Level::InitLevel(const char* levelPath, float dt, int iterations)
{
    this->iterations = iterations;

    this->dt = dt;

    gravity = GRAVITY;

    player.entityData.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    camera.zoom = 1.2f;

    float step = 1.0f / (float)GRID_SIZE;

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
                    player.gameObj.transform.position = tile->gameObj.transform.position;
                    player.spawnPos = player.gameObj.transform.position;
                }

                if(IsTypeInvalid(type)) level[l][i][j].type = TileType::VOID;

                //platforms...

                bool isPlatform = type > TileType::PLATFORM_START && type < TileType::PLATFORM_END;

                if(isPlatform)
                {
                    Platform platform = Platform();

                    platform.gameObj.hasBody = true;

                    float platformWidth = GRID_SIZE;
                    float platformHeight = GRID_SIZE;

                    platform.gravity = gravity;

                    switch (type)
                    {
                    case TileType::VERTICAL_MOVING_PLATFORM:
                    case TileType::HORIZONALT_MOVING_PLATFORM:
                    {
                        platformWidth = GRID_SIZE * 3.0f;
                        platformHeight = GRID_SIZE * 0.3f;
                    }
                    break;
                    
                    default: break;
                    }

                    platform.gameObj.transform = tile->gameObj.transform;
                    platform.gameObj.data = tile->gameObj.data;
                    platform.gameObj.direction = tile->gameObj.direction;

                    platform.ogPosition = platform.gameObj.transform.position;

                    platform.gameObj.hitboxes.push_back(Hitbox{{0,0}, {platformWidth, platformHeight}});

                    platform.gameObj.UpdateHitboxes();

                    float platformSpeed = 100.0f;

                    platform.SetTimer(0.3f);

                    platform.SetRespawnTimer(3.0f);

                    platform.textureIndex = level[l][i][j].textureIndex;

                    platform.variantIndex = level[l][i][j].variantIndex;

                    switch (type)
                    {
                    case TileType::HORIZONALT_MOVING_PLATFORM:
                    {
                        platform.type = PlatformType::MOVING_HORIZONTAL;

                        platform.gameObj.body.velocity.x = platformSpeed;

                        platform.updateRequired = true;
                    }
                    break;

                    case TileType::VERTICAL_MOVING_PLATFORM:
                    {
                        platform.type = PlatformType::MOVING_VERTICAL;

                        platform.gameObj.body.velocity.y = -platformSpeed;

                        platform.updateRequired = true;
                    }
                    break;

                    case TileType::FALLING_PLATFORM:
                    {
                        platform.type = PlatformType::FALLING;

                        platform.gameObj.body.hasGravity = true;
                    }
                    break;

                    case TileType::DISAPPEARING_PLATFORM:
                    {
                        platform.type = PlatformType::DISAPPEARING;

                        platform.SetRespawnTimer(1.0f);
                    }
                    break;

                    case TileType::VERTICAL_MOVING_SPIKE:
                    {
                        platform.type = PlatformType::MOVING_SPIKE_VERTICAL;

                        platformSpeed *= 1.5f;

                        platform.gameObj.body.velocity.y = -platformSpeed;

                        platform.updateRequired = true;

                        float factor = 0.8f;

                        platform.gameObj.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    case TileType::HORIZONTAL_MOVING_SPIKE:
                    {
                        platform.type = PlatformType::MOVING_SPIKE_HORIZONTAL;

                        platformSpeed *= 1.5f;

                        platform.gameObj.body.velocity.x = platformSpeed;

                        platform.updateRequired = true;

                        float factor = 0.8f;

                        platform.gameObj.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    case TileType::ROTATING_SPIKE_SINGLE:
                    {
                        platform.type = PlatformType::ROTATING_SPIKE_SINGLE;

                        platform.updateRequired = true;

                        platform.gameObj.body.velocity.x = platformSpeed;

                        if(platform.gameObj.data.flipX) platform.gameObj.body.velocity.x = -platformSpeed;

                        float size = GRID_SIZE * 0.5f;
                        
                        for(int h = 0; h < ROTATING_SPIKE_MAX_HITBOX; h++)
                        {
                            Vector2 offset = {0,0};

                            if(h >= 0)
                            {
                                float multiplier = (float)h * size;

                                offset.x = platform.gameObj.data.flipX ? -multiplier : multiplier;
                                offset.y = platform.gameObj.data.flipY ? multiplier : -multiplier;

                                platform.gameObj.AddSubHitbox(offset, {size,size});
                            }
                        }
                    }
                    break;

                    case TileType::ROTATING_SPIKE_DOUBLE:
                    {
                        platform.type = PlatformType::ROTATING_SPIKE_DOUBLE;

                        platform.updateRequired = true;

                        platform.gameObj.body.velocity.x = platformSpeed;

                        if(platform.gameObj.data.flipX) platform.gameObj.body.velocity.x = -platformSpeed;

                        float size = GRID_SIZE * 0.5f;
                        
                        for(int h = 0; h < ROTATING_SPIKE_MAX_HITBOX * 2.0f; h++)
                        {
                            if(h == ROTATING_SPIKE_MAX_HITBOX) continue;

                            Vector2 offset = {0,0};

                            float armSide = (h < ROTATING_SPIKE_MAX_HITBOX) ? 1.0f : -1.0f;
                            int localH = h % ROTATING_SPIKE_MAX_HITBOX;

                            if(localH >= 0)
                            {
                                float multiplier = (float)localH * size * armSide;

                                offset.x = platform.gameObj.data.flipX ? -multiplier : multiplier;
                                offset.y = platform.gameObj.data.flipY ? multiplier : -multiplier;

                                platform.gameObj.AddSubHitbox(offset, {size,size});
                            }
                        }
                    }
                    break;

                    default: break;
                    }

                    platformList.push_back(platform);
                }

                //enemies

                bool isEnemy = type > TileType::ENEMY_START && type < TileType::ENEMY_END;

                if(isEnemy)
                {
                    Enemy enemy = Enemy();

                    enemy.spawnPosition = tile->gameObj.transform.position;

                    enemy.gameObj.transform.position = enemy.spawnPosition;

                    enemy.gameObj.hasBody = true;

                    enemy.gameObj.body = {};

                    enemy.gameObj.hitboxes.push_back(Hitbox{{0,0}, {20,38}});

                    enemy.gameObj.UpdateHitboxes();

                    enemy.spawnData = tile->gameObj.data;
                    enemy.gameObj.data = enemy.spawnData;

                    enemy.gravity = gravity;

                    enemy.gameObj.body.hasGravity = true;

                    Hitbox jumpDetector = {
                        {0,0},
                        {enemy.gameObj.GetMainAABB().width * 0.9f, enemy.gameObj.GetMainAABB().height * 0.5f}
                    };

                    switch (type)
                    {
                    case TileType::ENEMY_DUMMY:
                    {
                        enemy.type = EnemyType::DUMMY;
                        enemy.testColor = ENEMY_DUMMY;
                    }
                    break;

                    case TileType::ENEMY_YUUKA:
                    {
                        enemy.type = EnemyType::YUUKA;
                        enemy.testColor = ENEMY_YUUKA;
                    }
                    break;
                    
                    default:
                    break;
                    }

                    enemy.gameObj.hitboxes.push_back(jumpDetector);

                    float nearPlayer = GRID_SIZE * 15.0f;

                    if(Vector2DistanceSqr(player.spawnPos, enemy.spawnPosition) <= nearPlayer * nearPlayer)
                        enemy.isActive = true;

                    enemyList.push_back(enemy);
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
                    tile->gameObj.canPlatformCollidePhysically = true;

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

    player.Shoot(dt);

    UpdateCamera(player.gameObj.transform.position, {0, -100});

    if(IsKeyPressed(KEY_R)) ResetLevel();
}

void Level::LowFrequencyUpdate()
{
    player.platformCache_update.clear();

    player.enemyCache.clear();
    
    float playerRadius = player.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER;
    float platformUpdateRadius = GRID_SIZE * 25.0f;

    for(int p = 0; p < platformList.size(); p++)
    {
        Platform& platform = platformList[p];

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

    float enemySpawnRadius = GRID_SIZE * 15.0f;
    float enemyDespawnRadius = GRID_SIZE * 17.0f;

    for(int e = 0; e < enemyList.size(); e++)
    {
        Enemy& enemy = enemyList[e];

        float despawnDistanceSqr = Vector2DistanceSqr(player.gameObj.transform.position, enemy.gameObj.transform.position);
        float spawnDistanceSqr = Vector2DistanceSqr(player.gameObj.transform.position, enemy.spawnPosition);

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

void Level::MediumFrequencyDiscreteUpdate()
{
    player.platformCache_physics.clear();
    player.platformCache_rendering.clear();

    player.enemyCache_physics.clear();

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    float renderRadius = GRID_SIZE * 15.0f;

    for(int p = 0; p < player.platformCache_update.size(); p++)
    {
        Platform* platform = player.platformCache_update[p];

        if(!platform) continue;

        bool isMovingPlatform = platform->type > PlatformType::MOVING_START && platform->type < PlatformType::MOVING_END;

        bool isRespawnPlatform = platform->type == PlatformType::FALLING || platform->type == PlatformType::DISAPPEARING;

        if(CheckCollisionCircles(
            player.gameObj.transform.position,
            player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
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
            collisionTileCheckRange
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

        enemy->UpdateAI(dt, player.gameObj.transform.position);
    }
}

void Level::HighFrequencyDiscreteUpdate()
{
    isGravityUp = gravity < 0;

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
        collisionTileCheckRange
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
            collisionTileCheckRange
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
        player.entityData.flipY = isGravityUp;

        for(int i = 0; i < platformList.size(); i++)
        {
            Platform& platform = platformList[i];

            if(!(platform.type == PlatformType::FALLING) || platform.updateRequired) continue;

            platform.gravity = gravity;
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

        for(int e = 0; e < player.enemyCache.size(); e++)
        {
            Enemy* enemy = player.enemyCache[e];

            if(!enemy) continue;

            CCD_CollisionResult result = CheckCollisionsBulletVsEntity_CCD(bullet, &enemy->gameObj, dt);

            if(result.collision)
            {
                bullet->didHit =  true;
            }
        }
    }
}

void Level::ResetLevel()
{
    if(gravity < 0) gravity *= -1;

    bool gravityUp = gravity < 0;

    isGravityUp = &gravityUp;

    player.gravity = gravity;
    player.entityData.flipY = gravityUp;

    for(int i = 0; i < platformList.size(); i++)
    {
        Platform& platform = platformList[i];

        if(!(platform.type == PlatformType::FALLING) || platform.updateRequired) continue;

        platform.gravity = gravity;
    }

    for(int i = 0; i < enemyList.size(); i++)
    {
        enemyList[i].isActive = true;
        enemyList[i].Respawn();
    }

    player.Respawn();
}

void Level::DrawLevel()
{
    double currentTime = GetTime();

    BeginMode2D(camera);

    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
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

            DrawExplosion(e->position.x, e->position.y, e->radius, e->renderData, 0, TILE_SCALE);
        }
    }

    DrawSprite(
        player.gameObj.transform,
        &player.weaponRenderData,
        player.entityData,
        player.currentFrame
    );

    //DebugDrawing();

    EndMode2D();

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

                DrawAABB(tileObj.GetMainAABB(), mainAABBColor);

                for(int h = 1; h < tileObj.hitboxes.size(); h++)
                {
                    DrawAABB(tileObj.GetSubAABB(h), MAGENTA);
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
    }
    
    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawCircleLines(b->posititon.x, b->posititon.y, b->radius, RED);

        DrawLine(b->posititon.x, b->posititon.y, b->posititon.x + b->velocity.x, b->posititon.y + b->velocity.y, BLUE);
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