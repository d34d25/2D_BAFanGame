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
                    player.phys.transform.position = tile->gameObj.transform.position;
                    player.spawnPos = player.phys.transform.position;
                }

                if(IsTypeInvalid(type)) level[l][i][j].type = TileType::VOID;

                //platforms...

                bool isPlatform = type > TileType::PLATFORM_START && type < TileType::PLATFORM_END;

                if(isPlatform)
                {
                    Platform platform = Platform();

                    platform.phys.hasBody = true;

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

                    platform.phys.transform = tile->gameObj.transform;
                    platform.phys.data = tile->gameObj.data;
                    platform.phys.direction = tile->gameObj.direction;

                    platform.ogPosition = platform.phys.transform.position;

                    platform.phys.hitboxes.push_back(Hitbox{{0,0}, {platformWidth, platformHeight}});

                    platform.phys.UpdateHitboxes();

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

                        platform.phys.body.velocity.x = platformSpeed;

                        platform.updateRequired = true;
                    }
                    break;

                    case TileType::VERTICAL_MOVING_PLATFORM:
                    {
                        platform.type = PlatformType::MOVING_VERTICAL;

                        platform.phys.body.velocity.y = -platformSpeed;

                        platform.updateRequired = true;
                    }
                    break;

                    case TileType::FALLING_PLATFORM:
                    {
                        platform.type = PlatformType::FALLING;

                        platform.phys.body.hasGravity = true;
                    }
                    break;

                    case TileType::DISAPPEARING_PLATFORM:
                    {
                        platform.type = PlatformType::DISAPPEARING;
                    }
                    break;

                    case TileType::VERTICAL_MOVING_SPIKE:
                    {
                        platform.type = PlatformType::MOVING_SPIKE_VERTICAL;

                        platformSpeed *= 1.5f;

                        platform.phys.body.velocity.y = -platformSpeed;

                        platform.updateRequired = true;

                        float factor = 0.8f;

                        platform.phys.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    case TileType::HORIZONTAL_MOVING_SPIKE:
                    {
                        platform.type = PlatformType::MOVING_SPIKE_HORIZONTAL;

                        platformSpeed *= 1.5f;

                        platform.phys.body.velocity.x = platformSpeed;

                        platform.updateRequired = true;

                        float factor = 0.8f;

                        platform.phys.AddSubHitbox({0,0}, {platformWidth * factor, platformHeight * factor});
                    }
                    break;

                    case TileType::ROTATING_SPIKE_SINGLE:
                    {
                        platform.type = PlatformType::ROTATING_SPIKE_SINGLE;

                        platform.updateRequired = true;

                        platform.phys.body.velocity.x = platformSpeed;

                        if(platform.phys.data.flipX) platform.phys.body.velocity.x = -platformSpeed;

                        float size = GRID_SIZE * 0.5f;
                        
                        for(int h = 0; h < ROTATING_SPIKE_MAX_HITBOX; h++)
                        {
                            Vector2 offset = {0,0};

                            if(h >= 0)
                            {
                                float multiplier = (float)h * size;

                                offset.x = platform.phys.data.flipX ? -multiplier : multiplier;
                                offset.y = platform.phys.data.flipY ? multiplier : -multiplier;

                                platform.phys.AddSubHitbox(offset, {size,size});
                            }
                        }
                    }
                    break;

                    case TileType::ROTATING_SPIKE_DOUBLE:
                    {
                        platform.type = PlatformType::ROTATING_SPIKE_DOUBLE;

                        platform.updateRequired = true;

                        platform.phys.body.velocity.x = platformSpeed;

                        if(platform.phys.data.flipX) platform.phys.body.velocity.x = -platformSpeed;

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

                                offset.x = platform.phys.data.flipX ? -multiplier : multiplier;
                                offset.y = platform.phys.data.flipY ? multiplier : -multiplier;

                                platform.phys.AddSubHitbox(offset, {size,size});
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

                    switch (type)
                    {
                    case TileType::ENEMY_DUMMY:
                        enemy.type = EnemyType::DUMMY;
                    break;
                    
                    default:
                    break;
                    }

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

    //% 2 for 30 fps
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

    player.bulletpool.get()->UpdateBullets(dt);

    player.Shoot(dt);

    UpdateCamera(player.phys.transform.position, {0, -100});
}

void Level::LowFrequencyUpdate()
{
    player.platformCache_update.clear();

    player.enemyCache.clear();
    
    float playerRadius = player.phys.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER;
    float platformUpdateRadius = GRID_SIZE * 25.0f;

    for(int p = 0; p < platformList.size(); p++)
    {
        Platform& platform = platformList[p];

        if(!platform.phys.hasBody) continue;

        if(platform.IsInactive())
        {
            platform.UpdateInactive(dt, 1);
            continue;
        }

        if(CheckCollisionCircles(
            player.phys.transform.position,
            playerRadius,
            platform.phys.transform.position,
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

        float despawnDistanceSqr = Vector2DistanceSqr(player.phys.transform.position, enemy.gameObj.transform.position);
        float spawnDistanceSqr = Vector2DistanceSqr(player.phys.transform.position, enemy.spawnPosition);

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

    float playerRadius = player.phys.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    float renderRadius = GRID_SIZE * 15.0f;

    for(int p = 0; p < player.platformCache_update.size(); p++)
    {
        Platform* platform = player.platformCache_update[p];

        if(!platform) continue;

        bool isMovingPlatform = platform->type > PlatformType::MOVING_START && platform->type < PlatformType::MOVING_END;

        bool isRespawnPlatform = platform->type == PlatformType::FALLING || platform->type == PlatformType::DISAPPEARING;

        if(CheckCollisionCircles(
            player.phys.transform.position,
            player.phys.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
            platform->phys.transform.position,
            renderRadius
        ))
        {
            player.platformCache_rendering.push_back(platform);
        }

        if(platform->updateRequired) platform->Update(dt, 1);

        if(isMovingPlatform)
        {
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

                        if(!objTile.canPlatformCollidePhysically) continue;

                        if(objTile.hitboxes.empty()) continue;

                        if(CheckCollisionCircles(
                            platform->phys.transform.position, 
                            platform->phys.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
                            objTile.transform.position,
                            objTile.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                        ))
                        {
                            if(CheckCollisionRecs(platform->phys.GetMainAABB(), objTile.GetMainAABB()))
                            {
                                SolveCollisions_Platform(&platform->phys, &objTile, (platform->type > PlatformType::MOVING_X && platform->type < PlatformType::MOVING_Y));
                            }
                        }
                    }
                }
            }
        }

        if(CheckCollisionCircles(
            player.phys.transform.position,
            playerRadius,
            platform->phys.transform.position, 
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
            player.phys.transform.position,
            playerRadius,
            enemy->gameObj.transform.position,
            GRID_SIZE * 3.0f
        ))
        {
            player.enemyCache_physics.push_back(enemy);
        }
    }
}

void Level::HighFrequencyDiscreteUpdate()
{
    bool isGravityUp = gravity < 0;

    float playerRadius = player.phys.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    TileRange playerTileRange = CalculateTileRange(
        player.phys.transform.position.x,
        player.phys.transform.position.y,
        collisionTileCheckRange
    );

    player.Update(dt, iterations);

    player.ResetFalgs();

    //player X pass

    player.phys.UpdatePositionX(dt, iterations);

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
                    player.phys.transform.position,
                    playerRadius,
                    tile.gameObj.transform.position,
                    tile.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                if(CheckCollisionRecs(player.phys.GetMainAABB(), tile.gameObj.GetMainAABB()))
                {
                    if(!IsTileOneWay(tile))
                    {
                        SolveCollisions(
                            &player.phys, &tile.gameObj, 
                            true, isGravityUp, 
                            tile.type == TileType::TRAMPOLINE,
                            false
                        );
                    }
                    else if(IsOneWayRightLeft(tile))
                    {
                        SolveCollisionsOneWayLeftRight(
                            &player.phys, &tile.gameObj,
                            tile.gameObj.direction == Direction::RIGHT
                        );
                    }
                }
            }
        }
    }

    //player Y pass

    player.phys.UpdatePositionY(dt, iterations);

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
                    player.phys.transform.position,
                    playerRadius,
                    objTile.transform.position,
                    objTile.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                bool isTileJumpTrigger = tile.isJumpTrigger;

                if(objTile.canEntityCollidePhysically)
                {
                    if(CheckCollisionRecs(player.phys.GetMainAABB(), objTile.GetMainAABB()))
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
                }

                //tile triggers and non solid tiles

                //if bugs with tirggers happen move this back into its own loop
                
                if(IsOneWayRightLeft(tile)) continue;

                if(CheckCollisionRecs(player.phys.GetMainAABB(), objTile.GetMainAABB()))
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
                                &player.phys,
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
                            ApplyWaterPhysics(&player.phys, isGravityUp);

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
                            if((IsAbove(player.phys.GetMainAABB(), objTile.GetMainAABB(), 1.0f) && !isGravityUp) || 
                            (IsBelow(player.phys.GetMainAABB(), objTile.GetMainAABB(), 1.0f) && isGravityUp))
                            {
                                if(!player.IsPressingDown())
                                {
                                    SolveCollisionsOneWayUpDown(&player.phys, &objTile, true, isGravityUp, true);

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
                            if(CheckCollisionRecs(player.phys.GetMainAABB(), objTile.GetSubAABB(h)))
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
                        player.phys.body.altVelocity = objTile.body.velocity;
                }

                if(CheckCollisionRecs(player.GetJumpDetector(), objTile.GetMainAABB()) && player.IsFalling())
                {
                    if(!IsOneWayUpDown(tile)) player.wasGrounded = true;
                    else if(IsOneWayUpDown(tile))
                    {
                        if(tile.gameObj.direction == Direction::UP &&
                            IsAbove(player.phys.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
                        )
                        {
                            player.wasGrounded = true;
                        }
                        else if(tile.gameObj.direction == Direction::DOWN &&
                            IsBelow(player.phys.GetMainAABB(), objTile.GetMainAABB(), 0.0f)
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

        for(int h = 1; h < platform->phys.hitboxes.size(); h++)
        {
            if(CheckCollisionRecs(player.phys.GetMainAABB(), platform->phys.GetSubAABB(h)))
            {
                if(IsPlatformSpike(platform->type))
                {
                    player.wasTouchingSpike = true;
                }
            }
        }

        if(!CheckCollisionCircles(
            player.phys.transform.position,
            player.phys.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
            platform->phys.transform.position,
            platform->phys.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
        )) continue;
        
        if(!IsPlatformSpike(platform->type))
        {
            if(CheckCollisionRecs(player.phys.GetMainAABB(), platform->phys.GetMainAABB()))
            {
                SolveCollisionsOneWayUpDown(
                    &player.phys, &platform->phys,
                    true, isGravityUp, true
                );
            }
        }

        if(CheckCollisionRecs(player.GetJumpDetector(), platform->phys.GetMainAABB()) && player.IsFalling())
        {
            if(!isGravityUp && IsAbove(player.phys.GetMainAABB(), platform->phys.GetMainAABB(), 0.0f) || 
            (isGravityUp && IsBelow(player.phys.GetMainAABB(), platform->phys.GetMainAABB(), 0.0f)))
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
            Platform& platform = platformList[i];

            if(!(platform.type == PlatformType::FALLING) || platform.updateRequired) continue;

            platform.gravity = gravity;
        }
    }

    if(!player.isTouchingSpike && player.wasTouchingSpike)
    {
        ResetLevel(&isGravityUp);
    }

    //booleans update

    player.UpdateFlags();
}

void Level::ResetLevel(bool* isGravityUp)
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
                DrawSprite(platform->phys, platformRenderData, frameToDraw);
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
                DrawRectangleRec(platform->phys.GetMainAABB(), platformColor);
            else
            {
                for(int h = 1; h < platform->phys.hitboxes.size(); h++)
                {
                    DrawRectangleRec(platform->phys.GetSubAABB(h), SPIKE);
                }
            }
        }
    }

    for(int i = 0; i < player.enemyCache.size(); i++)
    {
        Enemy* enemy = player.enemyCache[i];

        DrawRectangleRec(enemy->gameObj.GetMainAABB(), ENEMY_DUMMY);
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

            DrawExplosion(e->position.x, e->position.y, e->radius, e->renderData, 0, TILE_SCALE);
        }
    }

    DrawSprite(
        player.phys.transform,
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
        player.phys.transform.position.x,
        player.phys.transform.position.y,
        renderTileCheckRange
    );

    for(int i = 0; i < player.platformCache_rendering.size(); i++)
    {
        Platform* platform = player.platformCache_rendering[i];

        DrawLine(
            player.phys.transform.position.x, player.phys.transform.position.y, 
            platform->phys.transform.position.x, platform->phys.transform.position.y, 
            RED
        );

        DrawAABB(platform->phys.GetMainAABB(), RED);

        for(int h = 1; h < platform->phys.hitboxes.size(); h++)
        {
            DrawAABB(platform->phys.GetSubAABB(h), MAGENTA);
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
    }
    
    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawCircleLines(b->posititon.x, b->posititon.y, b->radius, RED);
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

    DrawAABB(player.phys.GetMainAABB(), ORANGE);

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