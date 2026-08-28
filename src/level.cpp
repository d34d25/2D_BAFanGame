#include "level.h"

#include <iostream>
#include <algorithm>

Level::~Level()
{
    ClearTileMatrix();

    ClearPlatformList();
}

void Level::InitLevel(const char* levelPath, const char* roomPath ,float dt, int iterations)
{
    retToTitle = false;

    this->iterations = iterations;

    this->dt = dt;

    enemiesBulletPools.clear();

    gravity = GRAVITY;

    camera.target = {0,0};

    camera.zoom = CAMERA_ZOOM;
    
    camera.rotation = 0.0f;

    selector.position = SetUIElementPosition(3,5);

    float step = 1.0f / (float)TILE_SIZE;

    camera.zoom = roundf(camera.zoom / step) * step;

    rangeLimits = GetTileRangeLimits();

    ClearTileMatrix();

    ClearPlatformList();

    LoadLevelData(levelPath, level, roomPath, rooms);

    platformList.reserve(800);

    enemyList.reserve(800);

    player.gameObj.transform.position = {0,0};

    for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
    {
        for(int i = 0; i < COLS; i++)
        {
            for(int j = 0; j < ROWS; j++)
            {
                Tile& tile = level[l][i][j];
                TileType type = tile.type;

                Vector2 tilePosition = GetTileCenter(i,j);

                if(type == TileType::PLAYER_SPAWN)
                {
                    Vector2 spawnPos = tilePosition;

                    player.InitPlayer(spawnPos, gravity, gravity < 0);

                    if(player.gameObj.GetMainAABB().height > TILE_SIZE) spawnPos.y -= player.gameObj.GetMainAABB().height * 0.25f;

                    player.gameObj.transform.position = spawnPos;
                    player.spawnPos = player.gameObj.transform.position;

                    player.gameObj.flipData.flipOffset = tile.flipData.flipOffset;
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

                    Transform2D platformInitTrasnform = {};

                    platformInitTrasnform.position = tilePosition;
                    platformInitTrasnform.angle = tile.angle;

                    platform.InitPlatform(
                        platformInitTrasnform,
                        tile.flipData,
                        gravity,
                        tile.textureIndex,
                        tile.variantIndex,
                        tile.paletteIndex
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
                    case TileType::ENEMY_DUMMY: enemy.type = EnemyType::DUMMY; break;

                    case TileType::ENEMY_AMAS_DRONE: enemy.type = EnemyType::AMAS_DRONE; break;

                    case TileType::ENEMY_AMAS_DRONE_B: enemy.type = EnemyType::AMAS_DRONE_B; break;

                    case TileType::ENEMY_BOMBER_DRONE: enemy.type = EnemyType::BOMBER_DRONE; break;

                    case TileType::ENEMY_SWEEPER_A: enemy.type = EnemyType::SWEEPER_A; break;

                    case TileType::ENEMY_SWEEPER_B: enemy.type = EnemyType::SWEEPER_B; break;

                    case TileType::ENEMY_AMAS_HEAVY: enemy.type = EnemyType::AMAS_HEAVY; break;

                    case TileType::ENEMY_HELMET_GANG: enemy.type = EnemyType::HELMET_GANG; break;

                    case TileType::ENEMY_YUUKA: enemy.type = EnemyType::YUUKA; break;
                    
                    default:
                    break;
                    }

                    /*
                    I should place normal tiles first
                    then place enemies, so I can run an Rec vs Rec collision
                    check, calculate the overlap, and move the spawn position
                    the amount needed to spawn them outside of any tile
                    */

                    Vector2 enemySpawnPos = tilePosition;

                    if(tile.flipData.flipY && tile.type == TileType::ENEMY_SWEEPER_B)
                    {
                        enemySpawnPos.y += 5;
                    }

                    enemy.InitEnemy(
                        enemySpawnPos,
                        tile.flipData,
                        tile.paletteIndex,
                        gravity
                    );

                    float nearPlayer = TILE_SIZE * 15.0f;

                    if(Vector2DistanceSqr(player.spawnPos, enemy.spawnPosition) <= nearPlayer * nearPlayer)
                        enemy.isActive = true;
                }

                //actual tiles
                if(IsNotRealTile(type))
                {
                    if(tile.type != TileType::VOID) tile.type = TileType::VOID;

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

                tile.neighborsTypes[0] = GetTileType(upLeftArray, level);
                tile.neighborsTypes[1] = GetTileType(upArray, level);
                tile.neighborsTypes[2] = GetTileType(upRightArray, level);

                tile.neighborsTypes[3] = GetTileType(rightArray, level);
                tile.neighborsTypes[4] = GetTileType(downRightArray, level);
                tile.neighborsTypes[5] = GetTileType(downArray, level);

                tile.neighborsTypes[6] = GetTileType(downLeftArray, level);
                tile.neighborsTypes[7] = GetTileType(leftArray, level);

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

                tile.isMiddle = true;

                tile.hitboxes.push_back(Hitbox{{0,0}, {TILE_SIZE, TILE_SIZE}});

                for(int h = 0; h < tile.hitboxes.size(); h++)
                {
                    tile.hitboxes[h].Update(tilePosition);
                }

                float treadmillVel = 50.0f;

                switch (type)
                {
                case TileType::SOLID:
                {
                    tile.canEntityCollidePhysically = true;
                    tile.canPlatformCollidePhysically = true;

                    tile.isJumpTrigger = true;
                }
                break;

                case TileType::PLATFORM_STOP:
                {
                    tile.canEntityCollidePhysically = false;
                    tile.canPlatformCollidePhysically = true;
                }
                break;
                
                case TileType::TRAMPOLINE:
                {
                    tile.canEntityCollidePhysically = true;
                    tile.canPlatformCollidePhysically = true;
                }
                break;

                case TileType::GRAVITY_CHANGER:
                {
                    tile.canEntityCollidePhysically = false;
                    tile.canPlatformCollidePhysically = false;
                }
                break;

                case TileType::TREADMILL_RIGHT:
                {
                    tile.canEntityCollidePhysically = true;
                    tile.canPlatformCollidePhysically = true;

                    tile.isJumpTrigger = true;

                    tile.treadmillVel = treadmillVel;
                }
                break;

                case TileType::TREADMILL_LEFT:
                {
                    tile.canEntityCollidePhysically = true;
                    tile.canPlatformCollidePhysically = true;

                    tile.isJumpTrigger = true;

                    tile.treadmillVel = -treadmillVel;
                }
                break;

                case TileType::ONE_WAY:
                {
                    tile.canEntityCollidePhysically = true;
                    tile.canPlatformCollidePhysically = true;

                    if(IsOneWayUpDown(tile)) tile.isJumpTrigger = true;
                }
                break;

                case TileType::LADDER:
                {
                    tile.canPlatformCollidePhysically = true;
                }
                break;

                default:
                {
                    tile.canEntityCollidePhysically = false;
                    tile.canPlatformCollidePhysically = false;
                    tile.isJumpTrigger = false;
                }
                break;
                }

                if(IsTileSpike(tile.type))
                {
                    tile.canEntityCollidePhysically = false;
                    tile.canPlatformCollidePhysically = false;

                    SpriteRenderData* spikeRenderData = GetTileActiveRenderData(TileType::SPIKE);

                    auto AddSpikeHitbox = [&](float widthFactor, float heightFactor, float offsetX, float offsetY)
                    {
                        if(tile.direction == Direction::LEFT || tile.direction == Direction::RIGHT)
                        {
                            std::swap(widthFactor, heightFactor);
                        }

                        Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                        Vector2 size = {tileMainAABB.width * widthFactor, tileMainAABB.height * heightFactor};

                        Vector2 offset = {0,0};

                        offset.x = offsetX;
                        offset.y = offsetY;

                        switch (tile.direction)
                        {
                            case Direction::DOWN: offset.y = -offsetY; break;

                            case Direction::LEFT: offset = {offsetY, offsetX}; break;
                            case Direction::RIGHT: offset = {-offsetY, offsetX}; break;

                            default: break;
                        }

                        tile.hitboxes.push_back(Hitbox{tilePosition, size, offset, true}); 
                    };

                    switch (tile.type)
                    {
                    case TileType::SPIKE:
                    {
                        float wFactor = 0.6f;
                        float hFactor = 0.4f;
    
                        float correctionY_A = 3;
                        float correctionY_B = -2;

                        AddSpikeHitbox(wFactor, hFactor,0 ,correctionY_A);
                        AddSpikeHitbox(hFactor * 0.5f, wFactor * 0.5f, 0 ,correctionY_B);
                    }
                        break;

                    case TileType::SPIKE_DOUBLE:
                    {
                        float wFactor = 0.3f;
                        float hFactor = 0.2f;

                        float correctionX_A = 3;

                        float correctionX_B = -correctionX_A;

                        float correctionY_A = 4;

                        float correctionY_B = 1;

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

                        float correctionY_A = 4;

                        float correctionY_B = 1;

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

    //safety check
    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = 0; i < COLS; i++)
        {
            for(int j = 0; j < ROWS; j++)
            {
                Tile& tile = level[l][i][j];

                if(IsNotRealTile(tile.type)) tile.type == TileType::VOID;
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
        enemyList[i].id = i;

        enemiesBulletPools[enemyList[i].id] = std::make_unique<BulletPool>(
            30, enemyList[i].bulletData
        );

        enemiesEffectPools[enemyList[i].id] = std::make_unique<EffectPool>(1, 1.0f, true);
            
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

    platformCache_update.reserve(100);
    platformCache_physics.reserve(100);
    platformCache_rendering.reserve(100);

    enemyCache.reserve(100);

    RoomChangeEvaluation();
}

/*
    where a flag lives (i.e when has to be reseted / updated)
    depends on where is the code that read those flags
    for example if the flag is read inside the high freq update,
    then the flag must live in the high freq update,
    if the flag is read once per frame then
    it must live in the medium freq update
    and if a flag is read at half the speed then the flag
    has to be reset / updated at the same rate

    with the exceptions of forced exits (i.e player.inLadder)
*/

void Level::UpdateLevel()
{
    lowFrequencyCounter++;

    if(lowFrequencyCounter % 4 == 0)
    {
        LowFrequencyUpdate(); //cache sorting
        lowFrequencyCounter = 0;
    }

    MediumFrequencyDiscreteUpdate_First();

    for(int iteraion = 0; iteraion < iterations; iteraion++)
    {
        if(LevelStarted()) HighFrequencyDiscreteUpdate();
        else RoomChangeEvaluation();
    }

    BulletsUpdate(); //bullets only

    MediumFrequencyDiscreteUpdate_Second();

    bool isShaking = screenShakeTimer < screenShakeDuration ||
    screenShakeOffset.x != 0.0f || screenShakeOffset.y != 0.0f;

    if(isShaking)
    {
        CalculateScreenShake();
    }

    UpdateCamera(player.gameObj.transform.position, {0, -25});

    camera.offset += screenShakeOffset;

    if(player.resetingZoom)
    {
        player.resetingZoom = false;
        camera.zoom = CAMERA_ZOOM;
    }
    else if(player.resetingLevel)
    {
        player.resetingLevel = false;
        ResetLevel();
    }

    //std::cout<<"respawn timer: "<<respawnTimer<<"\n";

    if(player.health <= 0)
    {
        TickRespawnTimer();

        if(!Respawning()) ResetLevel();
    }
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

    player.bulletpool.get()->Reset();
}

void Level::Pause()
{
    const int INCREMENT = TILE_SIZE * 2;

    if(player.pressingUp)
    {
        selector.position.y -= INCREMENT;

        player.pressingUp = false;

        player.pressingDown = false;
    }
    else if(player.pressingDown)
    {
        selector.position.y += INCREMENT;

        player.pressingDown = false;

        player.pressingUp = false;
    }

    if(selector.position.y <= MIN_SELECTOR_POS) selector.position.y = MIN_SELECTOR_POS;

    if(selector.position.y >= MAX_SELECTOR_POS) selector.position.y = MAX_SELECTOR_POS;

    switch ((int)selector.position.y)
    {
    case MIN_SELECTOR_POS: selector.selectedOption = MenuOptions::CONTINUE; break;

    case MIDDLE_SELECTOR_POS: selector.selectedOption = MenuOptions::RETURN_TO_TITLE; break;

    case MAX_SELECTOR_POS: selector.selectedOption = MenuOptions::RESTART; break;
    
    default: selector.selectedOption = MenuOptions::CONTINUE; break;
    }

    /*const char* debtext = "";

    switch (selector.selectedOption)
    {
    case MenuOptions::CONTINUE: debtext = "CONTINUE"; break;

    case MenuOptions::RETURN_TO_TITLE: debtext = "RETURN_TO_TITLE"; break;

    case MenuOptions::RESTART: debtext = "RESTART"; break;
    
    default:
        break;
    }

    std::cout<<debtext<<"\n";*/

    if(player.confirmationPressed)
    {
        switch (selector.selectedOption)
        {
        case MenuOptions::CONTINUE:
        {
            player.pausePressed = false;

            player.ResetInput();
        }
        break;

        case MenuOptions::RETURN_TO_TITLE:
        {
            player.stunTimer = player.maxStunTime;

            player.pausePressed = false;

            player.resetingLevel = true;

            retToTitle = true;
        } 
        break;

        case MenuOptions::RESTART:
        {
            player.stunTimer = player.maxStunTime;

            player.pausePressed = false;

            player.resetingLevel = true;
        }
        break;

        default:
            break;
        }
        
        selector.position.y = MIN_SELECTOR_POS;
    }
}

void Level::UpdateCamera(const Vector2 &target, const Vector2 &offset)
{
    int canvasWidth = gameplayCanvas.texture.width;
    int canvasHeight = gameplayCanvas.texture.height;

    int roomIndex = 0;

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
            (canvasWidth * 0.5f) / camera.zoom,
            (canvasHeight * 0.5f) / camera.zoom
        };

        Vector2 min = {
            currentRoom.x + halfScreenWorld.x,
            currentRoom.y + halfScreenWorld.y
        };

        Vector2 max = {
            (currentRoom.x + currentRoom.width) - halfScreenWorld.x,
            (currentRoom.y + currentRoom.height) - halfScreenWorld.y
        };

        if(currentRoom.width < (canvasWidth / camera.zoom))
        {
            desired.x = currentRoom.x + (currentRoom.width * 0.5f);
        }
        else
        {
            desired.x = Clamp(desired.x, min.x, max.x);
        }

        if(currentRoom.height < (canvasHeight / camera.zoom))
        {
            desired.y = currentRoom.y + (currentRoom.height * 0.5f);
        }
        else
        {
            desired.y = Clamp(desired.y, min.y, max.y);
        }
            
    }

    camera.target.x = desired.x;
    camera.target.y = desired.y;

    float zoom = camera.zoom;

    camera.target.x = floorf(camera.target.x * zoom) / zoom;
    camera.target.y = floorf(camera.target.y * zoom) / zoom;

    camera.offset = {floorf(canvasWidth * 0.5f), floorf(canvasHeight * 0.5f)};

    int mouseWheel = GetMouseWheelMove();

    float cameraFactor = 0.2f;

    if(IsKeyDown(KEY_LEFT_ALT))
    {
        if(mouseWheel > 0) camera.zoom += cameraFactor;
        else if(mouseWheel < 0) camera.zoom -= cameraFactor;

        float step = 1.0f / (float)TILE_SIZE;

        camera.zoom = roundf(camera.zoom / step) * step;

        camera.zoom = Clamp(camera.zoom, 0.25f, 15.25f);
    }
}

void Level::LowFrequencyUpdate()
{
    platformCache_update.clear();

    enemyCache.clear();
    
    float playerRadius = player.gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    if(currentRoomIndex > -1 && currentRoomIndex < enemyBuckets.size())
    {
        std::vector<Enemy>& activeEnemyBucket = enemyBuckets[currentRoomIndex];

        for(int e = 0; e < activeEnemyBucket.size(); e++)
        {
            Enemy& enemy = activeEnemyBucket[e];

            float despawnDistanceSqr = Vector2DistanceSqr(player.gameObj.transform.position, enemy.gameObj.transform.position);
            float spawnDistanceSqr = Vector2DistanceSqr(player.gameObj.transform.position, enemy.spawnPosition);

            if(despawnDistanceSqr > ENEMY_DESPAWN_RADIUS * ENEMY_DESPAWN_RADIUS)
            {
                enemy.isActive = false;
            }
            else if(!enemy.isActive)
            {
                if(spawnDistanceSqr <= ENEMY_DESPAWN_RADIUS * ENEMY_DESPAWN_RADIUS &&
                spawnDistanceSqr >= ENEMY_SPAWN_RADIUS * ENEMY_SPAWN_RADIUS)
                {
                    enemy.Respawn();
                    enemy.isActive = true;
                }
            }

            if(enemy.isActive)
            {
                enemyCache.push_back(&enemy);
            }
        }
    }

    if(currentRoomIndex > -1 && currentRoomIndex < platformBuckets.size())
    {
        std::vector<Platform>& activePLatformBucket = platformBuckets[currentRoomIndex];

        for(int p = 0; p < activePLatformBucket.size(); p++)
        {
            Platform& platform = activePLatformBucket[p];

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
                PLATFORM_UPDATE_RADIUS
            ))
            {
                platformCache_update.push_back(&platform);
            }
        }
    }
}

void Level::MediumFrequencyDiscreteUpdate_First()
{
    platformCache_physics.clear();
    platformCache_rendering.clear();

    enemyCache_physics.clear();

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    float renderRadius = TILE_SIZE * 28.0f;

    for(int p = 0; p < platformCache_update.size(); p++)
    {
        Platform* platform = platformCache_update[p];

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
            platformCache_rendering.push_back(platform);
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

            for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
            {
                for(int i = platformRange.startX; i <= platformRange.endX; i++)
                {
                    for(int j = platformRange.startY; j <= platformRange.endY; j++)
                    {
                        Tile& tile = level[l][i][j];

                        Vector2 tilePosition = GetTileCenter(i, j);

                        if(!tile.canPlatformCollidePhysically) continue;
                        
                        if(tile.hitboxes.empty()) continue;

                        Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                        if(CheckCollisionCircles(
                            platform->gameObj.transform.position, 
                            platform->gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
                            tilePosition,
                            tileMainAABB.width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                        ))
                        {
                            if(CheckCollisionRecs(platform->gameObj.GetMainAABB(), tileMainAABB))
                            {
                                SolveCollisions_Platform(platform->gameObj, tileMainAABB, (platform->type > PlatformType::MOVING_X && platform->type < PlatformType::MOVING_Y));
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
            TILE_SIZE * 5.0f
        ))
        {
            platformCache_physics.push_back(platform);
        }
    }

    //player vs enemy cache

    bool isBossPlayingIntro = false;

    for(int e = 0; e < enemyCache.size(); e++)
    {
        Enemy* enemy = enemyCache[e];

        if(!enemy) continue;

        if(CheckCollisionCircles(
            player.gameObj.transform.position,
            playerRadius,
            enemy->gameObj.transform.position,
            TILE_SIZE * 3.0f
        ))
        {
            enemyCache_physics.push_back(enemy);
        }

        enemy->UpdateRender(dt);

        BulletPool* enemyBulletPool = GetEnemyBulletPool(enemy->id);

        if(lowFrequencyCounter % enemy->aiFrameskip == 0)
        {
            if(enemy->type == EnemyType::YUUKA && enemy->genericCondition) TriggerScreenShake(0.5f, 2.5f); //5.0f

            if(enemy->playingIntro && enemyBulletPool) enemyBulletPool->Reset();

            enemy->UpdateAI(dt, player);

            enemy->ResetFlagsAI();
        }

        if(enemy->shooting && enemyBulletPool) enemy->Shoot(dt, enemyBulletPool);
        
        enemy->Update(dt, iterations);

        enemy->ResetFlags();

        if(enemy->isBoss && enemy->playingIntro) isBossPlayingIntro = true;
    }

    player.isBossPlayingIntro = isBossPlayingIntro;

    if(player.hurt) TriggerScreenShake(0.025f, 5.0f);

    if(player.health > 0)
    {
        player.UpdateRender(dt);

        player.Update(dt);
    }

    if(player.canMove) player.Shoot(dt);

    player.ResetFalgs();

    //tile render update

    double currentTime = GetTime();

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
                    tile.currentFrame = tile.textureIndex;

                    if(tileRenderData->spacing != 1)
                    {
                        tile.currentFrame = GetCurrentFrame(
                            tileRenderData->animationFrames,
                            tile.textureIndex,
                            tileRenderData->spacing,
                            tileRenderData->animationSpeed,
                            currentTime
                        );
                    }
                }
            }
        }
    }
}

void Level::MediumFrequencyDiscreteUpdate_Second()
{
    /*
        on enter trigger = !wasTouchingTrigger && isTouchingTrigger
        on exit trigger = wasTouchingTrigger && !isTouchingTrigger
    */

    if(player.touchedGravityChanger())
    {
        player.isGrounded = false;
        player.canJump = false;

        gravity *= -1;
        isGravityUp = gravity < 0;

        player.gravity = gravity;
        player.gameObj.flipData.flipY = isGravityUp;

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

    if(player.TouchedSpike())
    {
        player.health = 0;

        ShootEffect(
            dt,
            player.gameObj.transform.position,
            player.effectPool.get(), 
            100, 
             8
        );
    }

    if(player.TookDamage())
    {
        player.health--;

        player.ApplyInvul(1.25f);

        player.ApplyStun(0.5f);

        int knockback = 45;

        player.gameObj.body.velocity.x = player.gameObj.flipData.flipX ? knockback : -knockback;

        if(player.health <= 0)
        {
            player.health = 0;

            ShootEffect(
                dt,
                player.gameObj.transform.position,
                player.effectPool.get(), 
                100, 
                8
            );

            respawnTimer = 0.0f;
        }
    }

    player.UpdateFlags();

    //enemies

    for(int e = 0; e < enemyCache.size(); e++)
    {
        Enemy* enemy = enemyCache[e];

        if(!enemy) continue;

        if(enemy->TookDamage())
        {
            enemy->health--;

            enemy->ApplyInvul(0.75f);

            if(enemy->health <= 0)
            {
                enemy->health = 0;

                ShootEffect(dt, enemy->gameObj.transform.position, enemiesEffectPools[enemy->id].get(), 0);

                enemy->isActive = false;
            }
        }

        if(lowFrequencyCounter % enemy->aiFrameskip == 0) enemy->UpdateFlagsAI();

        enemy->UpdateFlags();
    }
}

void Level::HighFrequencyDiscreteUpdate()
{
    RoomChangeEvaluation();

    isGravityUp = gravity < 0;

    float playerRadius = player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
        collisionTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
    );

    //physics flags
    bool playerInWater = false;
    bool playerInWind = false;

    player.gameObj.body.UpdateVelocity(dt, iterations, gravity);

    //player X pass

    if(player.health > 0) player.gameObj.UpdatePositionX(dt, iterations);

    for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                Vector2 tilePosition = GetTileCenter(i,j);

                if(!tile.canEntityCollidePhysically) continue;

                if(!tile.isMiddle || tile.hitboxes.empty()) continue;

                Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                if(!CheckCollisionCircles(
                    player.gameObj.transform.position,
                    playerRadius,
                    tilePosition,
                    tileMainAABB.width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                if(CheckCollisionRecs(player.gameObj.GetMainAABB(), tileMainAABB))
                {
                    if(!IsTileOneWay(tile))
                    {
                        SolveCollisions(
                            player.gameObj,
                            tileMainAABB,
                            {tile.treadmillVel, 0},
                            true,
                            isGravityUp,
                            tile.type == TileType::TRAMPOLINE,
                            false
                        );
                    }
                    else if(IsOneWayRightLeft(tile))
                    {
                        SolveCollisionsOneWayLeftRight(
                            player.gameObj,
                            tileMainAABB,
                           {tile.treadmillVel, 0},
                           tile.direction == Direction::RIGHT 
                        );
                    }
                }
            }
        }
    }

    //player Y pass

    if(player.health > 0) player.gameObj.UpdatePositionY(dt, iterations);

    for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                Vector2 tilePosition = GetTileCenter(i,j);

                if(!tile.isMiddle || tile.hitboxes.empty()) continue;

                Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                if(!CheckCollisionCircles(
                    player.gameObj.transform.position,
                    playerRadius,
                    tilePosition,
                    tileMainAABB.width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                )) continue;

                bool isTileJumpTrigger = tile.isJumpTrigger;

                if(tile.canEntityCollidePhysically)
                {
                    if(CheckCollisionRecs(player.gameObj.GetMainAABB(), tileMainAABB))
                    {
                        if(!IsTileOneWay(tile))
                        {
                            SolveCollisions(
                            player.gameObj,
                            tileMainAABB,
                            {tile.treadmillVel, 0},
                            false,
                            isGravityUp,
                            tile.type == TileType::TRAMPOLINE,
                            false
                        );
                        }
                        else if(IsOneWayUpDown(tile))
                        {
                            SolveCollisionsOneWayUpDown(
                                player.gameObj,
                                tileMainAABB,
                                {tile.treadmillVel, 0},
                                tile.direction == Direction::UP,
                                isGravityUp, false
                            );
                        }
                    }

                    if(CheckCollisionRecs(player.GetCeilingDetector(), tileMainAABB))
                    {
                        if(IsOneWayUpDown(tile))
                        {
                            if(tile.direction == Direction::UP &&
                                IsAbove(player.gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                            )
                            {
                                player.hitCeiling = true;
                            }
                            else if(tile.direction == Direction::DOWN &&
                                IsBelow(player.gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                            )
                            {
                                player.hitCeiling = true;
                            }
                        }
                        else if(tile.canEntityCollidePhysically && !IsOneWayRightLeft(tile))
                        {
                            player.hitCeiling = true;
                        }
                    }
                }

                //tile triggers and non solid tiles

                //if bugs with tirggers happen move this back into its own loop
                
                if(IsOneWayRightLeft(tile)) continue;

                if(tile.type == TileType::LADDER && CheckCollisionRecs(player.GetLadderDetector(), tileMainAABB))
                {
                    player.inLadder = true;
                        
                    player.ladderSnapPosX = tilePosition.x;

                    bool isEdgeUp = tile.neighborsTypes[(int)NeighborDirection::UP] == TileType::VOID ||
                    tile.neighborsTypes[(int)NeighborDirection::UP] == TileType::DECO;

                    bool isEdgeDown = tile.neighborsTypes[(int)NeighborDirection::DOWN] == TileType::VOID ||
                    tile.neighborsTypes[(int)NeighborDirection::DOWN] == TileType::DECO;

                    if((!isGravityUp && isEdgeUp) || (isGravityUp && isEdgeDown))
                    {
                        if((IsAbove(player.gameObj.GetMainAABB(), tileMainAABB, ONE_WAY_TOLERANCE) && !isGravityUp) || 
                        (IsBelow(player.gameObj.GetMainAABB(), tileMainAABB, ONE_WAY_TOLERANCE) && isGravityUp))
                        {
                            if(!player.movingDown)
                            {
                                SolveCollisionsOneWayUpDown(
                                    player.gameObj,
                                    tileMainAABB,
                                    {tile.treadmillVel, 0}, 
                                    true,
                                    isGravityUp,
                                    true
                                );

                                player.inLadder = false;
                                isTileJumpTrigger = true;
                            }
                            else
                            {
                                //forces the player into a climbing state
                                //when doing a single press to climb down a ladder
                                player.climbing = true;

                                if(!isGravityUp) player.gameObj.transform.position.y += ONE_WAY_TOLERANCE;
                                else player.gameObj.transform.position.y -= ONE_WAY_TOLERANCE;
                            }
                        }
                        else 
                        {
                            isTileJumpTrigger = false;
                        }
                    }
                }

                if(CheckCollisionRecs(player.gameObj.GetMainAABB(), tileMainAABB))
                {
                    switch (tile.type)
                    {
                    case TileType::GRAVITY_CHANGER: player.isTouchingGravityChanger = true; break;
                    case TileType::WIND:
                    {
                        if(!playerInWind)
                        {
                            bool isEdgeUp = tile.direction == Direction::UP && 
                            (tile.neighborsTypes[(int)NeighborDirection::UP] == TileType::VOID ||
                            tile.neighborsTypes[(int)NeighborDirection::UP] == TileType::DECO);

                            bool isEdgeDown = tile.direction == Direction::DOWN &&
                            (tile.neighborsTypes[(int)NeighborDirection::DOWN] == TileType::VOID ||
                            tile.neighborsTypes[(int)NeighborDirection::DOWN] == TileType::DECO);

                            ApplyWind(
                                player.gameObj,
                                tileMainAABB,
                                tile.direction,
                                isEdgeUp,
                                isEdgeDown,
                                isGravityUp
                            );

                            playerInWind = true;

                            player.touchingWind = true;
                        }
                    }
                    break;

                    case TileType::WATER:
                    {
                        if(!playerInWater)
                        {
                            ApplyWaterPhysics(&player.gameObj, isGravityUp);

                            playerInWater = true;

                            player.touchingWater = true;
                        }
                    }
                    break;
                    
                    default:
                        break;
                    }

                    if(IsTileSpike(tile.type))
                    {
                        for(int h = 1; h < tile.hitboxes.size(); h++)
                        {
                            if(CheckCollisionRecs(player.gameObj.GetMainAABB(), tile.hitboxes[h].aabb))
                            {
                                player.isTouchingSpike = true;
                                break;
                            }
                        }
                    }
                }

                if(!isTileJumpTrigger) continue;

                if(tile.type == TileType::TREADMILL_LEFT || tile.type == TileType::TREADMILL_RIGHT)
                {
                    if(CheckCollisionRecs(player.GetTreadmillDetector(), tileMainAABB) && player.IsFalling())
                        player.gameObj.body.altVelocity = {tile.treadmillVel, 0};
                }

                if(CheckCollisionRecs(player.GetJumpDetector(), tileMainAABB) && player.IsFalling())
                {
                    if(!IsOneWayUpDown(tile)) player.isGrounded = true;
                    else if(IsOneWayUpDown(tile))
                    {
                        if(tile.direction == Direction::UP &&
                            IsAbove(player.gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                        )
                        {
                            player.isGrounded = true;
                        }
                        else if(tile.direction == Direction::DOWN &&
                            IsBelow(player.gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                        )
                        {
                            player.isGrounded = true;
                        }
                    }
                }
            }
        }
    }

    //player vs platforms

    for(int i = 0; i < platformCache_physics.size(); i++)
    {
        Platform* platform = platformCache_physics[i];

        for(int h = 1; h < platform->gameObj.hitboxes.size(); h++)
        {
            if(CheckCollisionRecs(player.gameObj.GetMainAABB(), platform->gameObj.GetSubAABB(h)))
            {
                if(IsPlatformSpike(platform->type))
                {
                    player.isTouchingSpike = true;
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
                    player.gameObj, platform->gameObj.GetMainAABB(), platform->gameObj.body.velocity,
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
                player.isGrounded = true;
            }
        }
    }
   
    //player vs enemies
    for(int i = 0; i < enemyCache_physics.size(); i++)
    {
        Enemy* enemy = enemyCache_physics[i];

        if(!enemy || player.health <= 0) continue;

        if(!CheckCollisionCircles(
            player.gameObj.transform.position,
            player.gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER,
            enemy->gameObj.transform.position,
            enemy->gameObj.GetMainAABB().width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
        )) continue;

        if(CheckCollisionRecs(player.gameObj.GetMainAABB(), enemy->gameObj.GetMainAABB()) && player.canTakeDamage)
        {
            player.hurt = true;
        }
    }

    //enemies vs tiles

    for(int e = 0; e < enemyCache.size(); e++)
    {
        Enemy* enemy = enemyCache[e];

        if(!enemy) continue;

        float enemyRadius = enemy->gameObj.GetMainAABB().height * REC_TO_CIRCLE_RADIUS_MULTIPLIER;

        TileRange enemyTileRange = CalculateTileRange(
            enemy->gameObj.transform.position.x,
            enemy->gameObj.transform.position.y,
            collisionTileCheckRange,
            rangeLimits.minX, rangeLimits.minY,
            rangeLimits.maxX, rangeLimits.maxY
        );

        bool enemyInWater = false;

        enemy->gameObj.body.UpdateVelocity(dt, iterations, enemy->gravity);

        //enemy x pass
        enemy->gameObj.UpdatePositionX(dt, iterations);

        for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
        {
            for(int i = enemyTileRange.startX; i <= enemyTileRange.endX; i++)
            {
                for(int j = enemyTileRange.startY; j <= enemyTileRange.endY; j++)
                {
                    if(enemy->canFly) continue;

                    Tile& tile = level[l][i][j];

                    Vector2 tilePosition = GetTileCenter(i,j);

                    if(!tile.canEntityCollidePhysically) continue;

                    if(!CanEnemyCollideWithTile(tile.type)) continue; 

                    if(!tile.isMiddle || tile.hitboxes.empty()) continue;

                    Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                    if(!CheckCollisionCircles(
                        enemy->gameObj.transform.position,
                        enemyRadius,
                        tilePosition,
                        tileMainAABB.width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                    )) continue;

                    if(CheckCollisionRecs(
                        enemy->gameObj.GetMainAABB(),
                        tileMainAABB
                    ))
                    {
                        enemy->isTouchingWall = true;

                        if(!IsTileOneWay(tile))
                        {
                            SolveCollisions(
                                enemy->gameObj,
                                tileMainAABB,
                                {tile.treadmillVel, 0},
                                true,
                                isGravityUp,
                                tile.type == TileType::TRAMPOLINE,
                                false
                            );
                        }
                        else if(IsOneWayRightLeft(tile))
                        {
                            SolveCollisionsOneWayLeftRight(
                                enemy->gameObj, tileMainAABB, {tile.treadmillVel,0},
                                tile.direction == Direction::RIGHT
                            );
                        }
                    }
                }
            }
        }

        //enemy y pass
        enemy->gameObj.UpdatePositionY(dt, iterations);

        for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
        {
            for(int i = enemyTileRange.startX; i <= enemyTileRange.endX; i++)
            {
                for(int j = enemyTileRange.startY; j <= enemyTileRange.endY; j++)
                {
                    if(enemy->canFly) continue;
                    
                    Tile& tile = level[l][i][j];

                    Vector2 tilePosition = GetTileCenter(i,j);

                    if(!CanEnemyCollideWithTile(tile.type)) continue; 

                    if(!tile.isMiddle || tile.hitboxes.empty()) continue;

                    Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                    if(!CheckCollisionCircles(
                        enemy->gameObj.transform.position,
                        enemyRadius,
                        tilePosition,
                        tileMainAABB.width * REC_TO_CIRCLE_RADIUS_MULTIPLIER
                    )) continue;

                    if(tile.canEntityCollidePhysically)
                    {
                        if(CheckCollisionRecs(
                        enemy->gameObj.GetMainAABB(),
                        tileMainAABB
                        ))
                        {
                            if(!IsTileOneWay(tile))
                            {
                                SolveCollisions(
                                    enemy->gameObj,
                                    tileMainAABB,
                                    {tile.treadmillVel, 0},
                                    false,
                                    false,
                                    tile.type == TileType::TRAMPOLINE,
                                    false
                                );
                            }
                            else if(IsOneWayUpDown(tile))
                            {
                                SolveCollisionsOneWayUpDown
                                (
                                    enemy->gameObj, tileMainAABB, {tile.treadmillVel,0},
                                    tile.direction == Direction::UP,
                                    false,
                                    false
                                );
                            }
                        }

                        if(CheckCollisionRecs(enemy->GetCeilingDetector(), tileMainAABB))
                        {
                            if(!IsOneWayUpDown(tile)) enemy->hitCeiling = true;
                            else if(IsOneWayUpDown(tile))
                            {
                                if(tile.direction == Direction::UP &&
                                    IsAbove(enemy->gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                                )
                                {
                                    enemy->hitCeiling = true;
                                }
                                else if(tile.direction == Direction::DOWN &&
                                    IsBelow(enemy->gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                                )
                                {
                                    enemy->hitCeiling = true;
                                }
                            }
                        }
                    }
                    
                    if(IsOneWayRightLeft(tile)) continue;
                    
                    if(CheckCollisionRecs(enemy->gameObj.GetMainAABB(), tileMainAABB))
                    {
                        switch (tile.type)
                        {
                        case TileType::WATER:
                        {
                            if(!enemyInWater)
                            {
                                ApplyWaterPhysics(&enemy->gameObj, false);
                                
                                enemyInWater = true;

                                enemy->inWater = true;
                            }
                        }
                        break;
                        
                        default:
                            break;
                        }
                    }

                    if(!tile.isJumpTrigger) continue;

                    if(CheckCollisionRecs(enemy->GetJumpDetector(), tileMainAABB))
                    {
                        if(!IsOneWayUpDown(tile)) enemy->isGrounded = true;
                        else if(IsOneWayUpDown(tile))
                        {
                            if(tile.direction == Direction::UP &&
                                IsAbove(enemy->gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                            )
                            {
                                enemy->isGrounded = true;
                            }
                            else if(tile.direction == Direction::DOWN &&
                                IsBelow(enemy->gameObj.GetMainAABB(), tileMainAABB, 0.0f)
                            )
                            {
                                enemy->isGrounded = true;
                            }
                        }
                    }
                }
            }
        }

    }
}

void Level::RoomChangeEvaluation()
{
    Vector2 playerFuturePos = {
        player.gameObj.transform.position.x + player.gameObj.body.GetFinalVelocity().x * (dt / iterations),
        player.gameObj.transform.position.y + player.gameObj.body.GetFinalVelocity().y * (dt / iterations)
    };

    int roomIndex = 0;

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
}

void Level::BulletsUpdate()
{
    player.bulletpool.get()->UpdateBullets(dt);

    player.effectPool.get()->UpdateEffects(dt);

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* bullet = player.bulletpool->activeBullets[i];

        if(!bullet) continue;
        
        if(currentRoomIndex > -1 && currentRoomIndex < rooms.size())
        {
            Rectangle& currentRoom = rooms[currentRoomIndex].aabb;

            if(!CheckCollisionPointRec(bullet->posititon, currentRoom)) bullet->didHit = true;
        }

        for(int e = 0; e < enemyCache.size(); e++)
        {
            Enemy* enemy = enemyCache[e];

            if(!enemy || enemy->health <= 0) continue;

            if(CheckCollisionCircleRec(
                bullet->posititon, bullet->radius, enemy->gameObj.GetMainAABB()
            ) && enemy->canTakeDamage)
            {
                if(!player.bulletpool.get()->pierces) bullet->didHit =  true;

                enemy->hurt = true;
            }
        }

        if(player.bulletpool->explodes)
        {
            TileRange bulletRange = CalculateTileRange(
                bullet->posititon.x, bullet->posititon.y,
                2.0f, rangeLimits.minX, rangeLimits.minY,
                rangeLimits.maxX, rangeLimits.maxY 
            );

            for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
            {
                for(int i = bulletRange.startX; i <= bulletRange.endX; i++)
                {
                    for(int j = bulletRange.startY; j <= bulletRange.endY; j++)
                    {
                        Tile& tile = level[l][i][j];

                        if(tile.hitboxes.empty()) continue;

                        Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                        if(tile.canEntityCollidePhysically)
                        {
                            if(CheckCollisionCircleRec(
                                bullet->posititon, bullet->radius,
                                tileMainAABB
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

    for(int i = 0; i < enemyCache.size(); i++)
    {
        Enemy* enemy = enemyCache[i];

        if(!enemy) continue;

        if(player.bulletpool->explodes)
        {
            for(int x = 0; x < player.bulletpool->activeExplosions.size(); x++)
            {
                Explosion* ex = player.bulletpool->activeExplosions[x];
                        
                if(!ex) continue;

                if(CheckCollisionCircleRec(
                    ex->position, ex->radius, enemy->gameObj.GetMainAABB()
                ) && enemy->canTakeDamage)
                {
                    enemy->hurt = true;
                }
            }
        }
    }

    for(auto& [id, enemyBulletPool] : enemiesBulletPools)
    {
        if(!enemyBulletPool) continue;

        enemyBulletPool->UpdateBullets(dt);

        for(int b = 0; b < enemyBulletPool->activeBullets.size(); b++)
        {
            Bullet* bullet = enemyBulletPool->activeBullets[b];

            if(!bullet) continue;

            TileRange bulletRange = CalculateTileRange(
                bullet->posititon.x, bullet->posititon.y,
                2.0f, rangeLimits.minX, rangeLimits.minY,
                rangeLimits.maxX, rangeLimits.maxY 
            );

            if(currentRoomIndex > -1 && currentRoomIndex < rooms.size())
            {
                Rectangle& currentRoom = rooms[currentRoomIndex].aabb;

                if(!CheckCollisionPointRec(bullet->posititon, currentRoom)) bullet->didHit = true;
            }

            if(CheckCollisionCircleRec(
                bullet->posititon, bullet->radius, player.gameObj.GetMainAABB()
            ) && player.canTakeDamage && player.health > 0)
            {
                bullet->didHit = true;

                player.hurt = true;
            }

            if(enemyBulletPool->explodes)
            {
                for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
                {
                    for(int c = bulletRange.startX; c <= bulletRange.endX; c++)
                    {
                        for(int r = bulletRange.startY; r <= bulletRange.endY; r++)
                        {
                            Tile& tile = level[l][c][r];

                            if(tile.hitboxes.empty()) continue;

                            Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                            if(tile.canEntityCollidePhysically)
                            {
                                if(CheckCollisionCircleRec(
                                    bullet->posititon, bullet->radius,
                                    tileMainAABB
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

        if(enemyBulletPool->explodes)
        {
            for(int x = 0; x < enemyBulletPool->activeExplosions.size(); x++)
            {
                Explosion* ex = enemyBulletPool->activeExplosions[x];

                if(!ex) continue;

                if(CheckCollisionCircleRec(
                    ex->position, ex->radius, player.gameObj.GetMainAABB()
                ) && player.canTakeDamage)
                {
                    player.hurt = true;
                }
            }
        }
    }

    for(auto& [id, enemyEffectPool] : enemiesEffectPools)
    {
        if(!enemyEffectPool) continue;

        enemyEffectPool->UpdateEffects(dt);
    }
}

void Level::ResetLevel()
{
    if(gravity < 0) gravity *= -1;

    bool gravityUp = gravity < 0;

    player.gravity = gravity;
    player.gameObj.flipData.flipY = gravityUp;

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
            
            float despawnDistanceSqr = Vector2DistanceSqr(camera.target, enemy.gameObj.transform.position);
            float spawnDistanceSqr = Vector2DistanceSqr(camera.target, enemy.spawnPosition);

            if(despawnDistanceSqr > ENEMY_DESPAWN_RADIUS * ENEMY_DESPAWN_RADIUS)
            {
                enemy.isActive = false;
            }
            else if(!enemy.isActive)
            {
                if(spawnDistanceSqr <= ENEMY_DESPAWN_RADIUS * ENEMY_DESPAWN_RADIUS &&
                spawnDistanceSqr >= ENEMY_SPAWN_RADIUS * ENEMY_SPAWN_RADIUS)
                {
                    enemy.isActive = true;
                }
            }
        }
    }

    for(auto& [id, enemyBulletPool] : enemiesBulletPools)
    {
        if(!enemyBulletPool) continue;

        enemyBulletPool.get()->Reset();
    }

    for(auto& [id, enemiesEffectPool] : enemiesEffectPools)
    {
        if(!enemiesEffectPool) continue;

        enemiesEffectPool.get()->Reset();
    }

    player.Respawn();

    levelStartTimer = 0.0f;

    respawnTimer = 0.0f;

    retToTitle = false;
}

void Level::DrawLevel()
{
    double currentTime = GetTime();

    BeginTextureMode(gameplayCanvas);

    Room& currentRoom = rooms[currentRoomIndex];

    Color backgroundColor = GRAY;

    if(!environmentPalettes.empty())
    {
        if(currentRoom.currentPaletteIndex >= 0 && currentRoom.currentPaletteIndex < environmentPalettes.size())
        {
            if(currentRoom.currentColorIndex >= 0 && currentRoom.currentColorIndex < environmentPalettes.at(currentRoom.currentPaletteIndex).size())
            {
                backgroundColor =  environmentPalettes.at(currentRoom.currentPaletteIndex).at(currentRoom.currentColorIndex);
            }
        }
    }

    TileRange playerTileRange = CalculateTileRange(
        camera.target.x,
        camera.target.y,
        renderTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
    );

    lastPaletteIndex = -1;
    lastPaletteList = nullptr;

    ClearBackground(backgroundColor);

    Texture2D* activeBackground = GetActvieBackground(currentRoom.backgroundTextureIndex);

    BeginShaderMode(paletteShader);

    if(activeBackground)
    {
        ChangePalette(currentRoom.currentPaletteIndex, &environmentPalettes);

        DrawTexture(*activeBackground, 0,0, WHITE);
    }

    BeginMode2D(camera);

    //background

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            Tile& tile = level[BACKGROUND_LAYER][i][j];

            Vector2 tilePosition = GetTileCenter(i,j);

            if(tile.type != TileType::DECO) continue;

            SpriteRenderData* tileRenderData = GetTileActiveRenderData(tile.type, tile.variantIndex);

            if(tileRenderData)
            {
                ChangePalette(tile.paletteIndex, &environmentPalettes);

                if(tile.currentFrame >= 0 && tile.currentFrame < (int)tileRenderData->animationFrames.size())
                {
                    DrawSprite(tilePosition, tile.angle, tileRenderData, tile.flipData ,tile.currentFrame);
                }
            }
        }
    }

    //gameplay drawing

    for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                Vector2 tilePosition = GetTileCenter(i,j);

                if(IsNotRealTile(tile.type)) continue;

                SpriteRenderData* tileRenderData = GetTileActiveRenderData(tile.type, tile.variantIndex);

                if(tileRenderData)
                {
                    ChangePalette(tile.paletteIndex, &environmentPalettes);

                    if(tile.currentFrame >= 0 && tile.currentFrame < (int)tileRenderData->animationFrames.size())
                    {
                        DrawSprite(tilePosition, tile.angle, tileRenderData, tile.flipData ,tile.currentFrame);
                    }
                }
            }
        }
    }

    for(int i = 0; i < platformCache_rendering.size(); i++)
    {
        Platform* platform = platformCache_rendering[i];

        if(platform->renderData)
        {
            ChangePalette(platform->paletteIndex, &environmentPalettes);

            if(platform->type == PlatformType::FALLING || platform->type == PlatformType::DISAPPEARING)
            {
                DrawSprite(platform->gameObj, platform->renderData, platform->textureIndex);
            }
            else
            {
                int frameToDraw = platform->textureIndex;

                if(platform->renderData->spacing != 1)
                {
                    frameToDraw = GetCurrentFrame(
                        platform->renderData->animationFrames,
                        platform->textureIndex,
                        platform->renderData->spacing,
                        platform->renderData->animationSpeed,
                        currentTime
                    );
                }

                if(frameToDraw >= 0 && frameToDraw < (int)platform->renderData->animationFrames.size())
                {
                    if(platform->type == PlatformType::ROTATING_SPIKE_DOUBLE || 
                        platform->type == PlatformType::ROTATING_SPIKE_SINGLE)
                    {
                        for(int h = 0; h < platform->gameObj.hitboxes.size(); h++)
                        {
                            Rectangle& aabb = platform->gameObj.hitboxes[h].aabb;

                            float centerX = aabb.x + aabb.width * 0.5f;
                            float centerY = aabb.y + aabb.height * 0.5f;

                            DrawSprite(platform->renderData, centerX, centerY, frameToDraw,
                            platform->gameObj.flipData.flipX, platform->gameObj.flipData.flipY);
                        }
                    }     
                    else
                    {
                        DrawSprite(platform->gameObj, platform->renderData, frameToDraw);
                    }
                    
                }
            }
        }
    }

    int flickeringRate = 2;

    for(int i = 0; i < enemyCache.size(); i++)
    {
        Enemy* enemy = enemyCache[i];
        
        if(!enemy->enemyRenderData) DrawRectangleRec(enemy->gameObj.GetMainAABB(), enemy->testColor);
        else
        {
            ChangePalette(enemy->characterPaletteIndex, &spritePalettes);

            if(!enemy->canTakeDamage)
            {
                if(lowFrequencyCounter % flickeringRate == 0) DrawSprite(enemy->gameObj, enemy->enemyRenderData, enemy->characterCurrentFrame);
            }
            else
            {
                DrawSprite(enemy->gameObj, enemy->enemyRenderData, enemy->characterCurrentFrame);
            }
            
        }
    }

    ChangePalette(player.characterCurrentPalette, &spritePalettes);

    if(player.health > 0)
    {
        if(!player.canTakeDamage)
        {
            if((lowFrequencyCounter % flickeringRate == 0) && LevelStarted()) DrawSprite(player.gameObj, player.characterRenderData, player.characterCurrentFrame);
        }
        else
        {
            if(LevelStarted()) DrawSprite(player.gameObj, player.characterRenderData, player.characterCurrentFrame);
        }
    }
    
    EndShaderMode();

    if(player.health <= 0)
    {
        for(int i = 0; i < player.effectPool->activeEffects.size(); i++)
        {
            Effect* e = player.effectPool->activeEffects[i];

            if(!e) continue;

            DrawBullet(e->position.x, e->position.y, e->radius, BULLET_COLOR);
        }
    }

    for(int i = 0; i < player.bulletpool->activeBullets.size(); i++)
    {
        Bullet* b = player.bulletpool->activeBullets[i];

        if(!b) continue;

        DrawBullet(b->posititon.x, b->posititon.y, b->radius, b->mainColor);
    }

    if(!player.bulletpool->explosions.empty())
    {
        for(int i = 0; i < player.bulletpool->activeExplosions.size(); i++)
        {
            Explosion* e = player.bulletpool->activeExplosions[i];

            if(!e) continue;

            DrawExplosion(e->position.x, e->position.y, e->radius);
        }
    }

    for(auto& [id, enemyBulletPool] : enemiesBulletPools)
    {
        if(!enemyBulletPool) continue;

        for(int b = 0; b < enemyBulletPool->activeBullets.size(); b++)
        {
            Bullet* bullet = enemyBulletPool->activeBullets[b];

            if(!bullet) continue;

            DrawBullet(bullet->posititon.x, bullet->posititon.y, bullet->radius, bullet->mainColor);
        }

        if(!enemyBulletPool->explosions.empty())
        {
            for(int i = 0; i < enemyBulletPool->activeExplosions.size(); i++)
            {
                Explosion* explosion = enemyBulletPool->activeExplosions[i];

                if(!explosion) continue;

                DrawExplosion(explosion->position.x, explosion->position.y, explosion->radius);
            }
        }
    }

    for(auto& [id, enemyEffectPool] : enemiesEffectPools)
    {
        if(!enemyEffectPool) continue;

        for(Effect* e : enemyEffectPool->activeEffects)
        {
            if(!e) continue;

            DrawBullet(e->position.x, e->position.y, e->radius, BULLET_COLOR);
        }
    }

    if(debugDrawing) DebugDrawing();

    EndMode2D();

    if(!LevelStarted())
    {
        Vector2 readyTextPos = SetUIElementPosition(8,2);

        if(lowFrequencyCounter % 4 == 0) DrawText("Ready", readyTextPos.x, readyTextPos.y, 1, WHITE);
    }
    else if(paused) PauseDrawing();

    EndTextureMode();

    //UI
    DrawLevelUI();

    ClearBackground(BLACK);

    float scale = fminf((float)GetScreenWidth() / NATIVE_WIDTH, 
    (float)GetScreenHeight() / NATIVE_HEIGHT);

    float offsetX = ((float)GetScreenWidth() - ((float)NATIVE_WIDTH * scale)) * 0.5f;
    float offsetY = ((float)GetScreenHeight() - ((float)NATIVE_HEIGHT * scale)) * 0.5f;

    Rectangle sourceGameplayRec = {
        0,
        0, 
        (float)gameplayCanvas.texture.width,
        (float)-gameplayCanvas.texture.height
    };

    Rectangle destGameplayRec = {
        offsetX,
        offsetY,
        gameplayCanvas.texture.width * scale,
        gameplayCanvas.texture.height * scale
    };

    Rectangle sourceUiRec = {
        0,0,
        (float)uiCanvas.texture.width,
        (float)-uiCanvas.texture.height
    };

    Rectangle destUiRec = {
        offsetX,
        offsetY + (gameplayCanvas.texture.height * scale),
        uiCanvas.texture.width * scale,
        uiCanvas.texture.height * scale
    };

    DrawTexturePro(
        gameplayCanvas.texture,
        sourceGameplayRec,
        destGameplayRec,
        {0,0},
        0.0f,
        WHITE
    );

    DrawTexturePro(
        uiCanvas.texture,
        sourceUiRec,
        destUiRec,
        {0,0},
        0.0f,
        WHITE
    );

    //DebugTextDrawing();
}

void Level::DrawLevelUI()
{
    BeginTextureMode(uiCanvas);

    ClearBackground(WHITE);

    Vector2 midCanvas = {
        floorf(uiCanvas.texture.width * 0.5f),
        floorf(uiCanvas.texture.height * 0.5f),
    };

    BeginShaderMode(paletteShader);

    ChangePalette(player.characterCurrentPalette, &spritePalettes);
    
    if(player.currentPortrait > -1 && player.health > 0)
    {
        Vector2 portraitPos = SetUIElementPosition(18,2);

        DrawSprite(
            &portraits[player.currentPortrait],
            portraitPos.x,
            portraitPos.y,
            player.currentPortraitFrame
        );
    }

    int step = 2;

    int healthPosStart = 3;

    ChangePalette(7, &spritePalettes);

    for(int e = 0; e < enemyCache.size(); e++)
    {
        Enemy* enemy = enemyCache[e];

        if(!enemy || enemy->type != EnemyType::YUUKA) continue;

        for(int i = healthPosStart; i < healthPosStart + enemy->health * step; i += step)
        {
            Vector2 healthPointPos = SetUIElementPositionCentered(i, 2);

            DrawSprite(&uiElements[0], healthPointPos.x, healthPointPos.y, 0);
        }   
    }

    ChangePalette(11, &spritePalettes);

    for(int i = healthPosStart; i < healthPosStart + player.health * step; i += step)
    {
        Vector2 healthPointPos = SetUIElementPositionCentered(i, 0);

        DrawSprite(&uiElements[0], healthPointPos.x, healthPointPos.y, 0);
    }

    EndShaderMode();

    Vector2 hpTextPos = SetUIElementPosition(1,0);

    DrawText("HP", hpTextPos.x, hpTextPos.y, TILE_SIZE, BLACK);

    Vector2 bossHpTextPos = SetUIElementPosition(1,2);

    DrawText("HP", bossHpTextPos.x, bossHpTextPos.y, TILE_SIZE, BLACK);

    DrawRectangleLinesEx(
        {0,0,CANVAS_WIDTH, UI_CANVAS_HEIGHT},
        1, BLACK
    );

    if(debugDrawing)
    {
        float dynamicThickness = 1.0f / camera.zoom;

        for(int i = 0; i <= CANVAS_WIDTH; i+= TILE_SIZE)
        {
            DrawLineEx(
                {(float)i, 0.0f},
                {(float)i, (float)UI_CANVAS_HEIGHT},
                dynamicThickness,
                GRAY
            );
        }

        for(int i = 0; i <= UI_CANVAS_HEIGHT; i+= TILE_SIZE)
        {
            DrawLineEx(
                {0.0f, (float)i},
                {(float)CANVAS_WIDTH, (float)i},
                dynamicThickness,
                GRAY
            );
        }
    }

    EndTextureMode();
}

void Level::PauseDrawing()
{
    Vector2 pauseTextPos = SetUIElementPosition(8, 2);

    DrawText("PAUSE", pauseTextPos.x,  pauseTextPos.y, 1, WHITE);

    Vector2 pauseMenuRecPos = SetUIElementPosition(2,4);

    Rectangle pauseMenuRec = {
        pauseMenuRecPos.x,
        pauseMenuRecPos.y,
        TILE_SIZE * 16,
        TILE_SIZE * 7
    };

    DrawRectangleRec(pauseMenuRec, BLACK);

    DrawRectangleLinesEx(pauseMenuRec, 1, WHITE);

    Vector2 contTextPos = SetUIElementPosition(4,CONTINUE_TEXT_POS_Y);

    DrawText("Continue", contTextPos.x, contTextPos.y, 1, WHITE);

    Vector2 retTextPos = SetUIElementPosition(4, RETURN_TEXT_POS_Y);

    DrawText("Return to title", retTextPos.x, retTextPos.y, 1, WHITE);

    Vector2 restartTextPos = SetUIElementPosition(4, RESTART_TEXT_POS_Y);

    DrawText("Restart", restartTextPos.x, restartTextPos.y, 1, WHITE);

    DrawText(">", selector.position.x, selector.position.y, 1, WHITE);

    if(debugDrawing)
    {
        for(int i = 0; i <= CANVAS_WIDTH; i+= TILE_SIZE)
        {
            DrawLineEx(
                {(float)i, 0.0f},
                {(float)i, (float)GAMEPLAY_CANVAS_HEIGHT},
                1.0f,
                MAGENTA
            );
        }

        for(int i = 0; i <= GAMEPLAY_CANVAS_HEIGHT; i+= TILE_SIZE)
        {
            DrawLineEx(
                {0.0f, (float)i},
                {(float)CANVAS_WIDTH, (float)i},
                1.0f,
                MAGENTA
            );
        }
    }
}

void Level::DebugDrawing()
{
    int worldWidth = COLS * TILE_SIZE;
    int worldHeight = ROWS * TILE_SIZE;

    //grid

    Color gridColor = GRAY;
    gridColor.a = 127;

    float dynamicThickness = 1.0f / camera.zoom;

    for(int i = 0; i <= worldWidth; i+= TILE_SIZE)
    {
        DrawLineEx(
            {(float)i, 0.0f},
            {(float)i, (float)worldHeight},
            dynamicThickness,
            GRAY
        );
    }

    for(int i = 0; i <= worldHeight; i+= TILE_SIZE)
    {
        DrawLineEx(
            {0.0f, (float)i},
            {(float)worldWidth, (float)i},
            dynamicThickness,
            GRAY
        );
    }

    //tiles

    for(int i = 0; i < platformCache_rendering.size(); i++)
    {
        Platform* platform = platformCache_rendering[i];

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

    TileRange playerTileRange = CalculateTileRange(
        player.gameObj.transform.position.x,
        player.gameObj.transform.position.y,
        collisionTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
    );

    for(int l = GAMEPLAY_LAYER_START; l <= GAMEPLAY_LAYER_END; l++)
    {
        for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
        {
            for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                if(IsNotRealTile(tile.type)) continue;

                if(tile.hitboxes.empty()) continue;

                Color collisionCheckColor = {255,0,0,172};

                DrawRectangle(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE, collisionCheckColor);
            }
        }
    }

    TileRange playerRenderTileRange = CalculateTileRange(
        camera.target.x,
        camera.target.y,
        renderTileCheckRange,
        rangeLimits.minX, rangeLimits.minY,
        rangeLimits.maxX, rangeLimits.maxY
    );

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = playerRenderTileRange.startX; i <= playerRenderTileRange.endX; i++)
        {
            for(int j = playerRenderTileRange.startY; j <= playerRenderTileRange.endY; j++)
            {
                Tile& tile = level[l][i][j];

                Vector2 tilePosition = GetTileCenter(i,j);

                if(IsNotRealTile(tile.type)) continue;

                if(tile.hitboxes.empty()) continue;

                Rectangle& tileMainAABB = tile.hitboxes[0].aabb;

                Color mainAABBColor = RED;

                if(level[l][i][j].type == TileType::PLATFORM_STOP) mainAABBColor = BLUE;

                DrawAABB(tileMainAABB, mainAABBColor, dynamicThickness);

                for(int h = 1; h < tile.hitboxes.size(); h++)
                {
                    DrawAABB(tile.hitboxes[h].aabb, MAGENTA, dynamicThickness);
                }

                Vector2 lineEnd = tilePosition;

                float halfW = tileMainAABB.width * 0.5f;
                float halfH = tileMainAABB.height * 0.5f;

                switch (tile.direction)
                {
                case Direction::UP: lineEnd.y -= halfH; break;
                case Direction::DOWN: lineEnd.y += halfH; break;
                case Direction::LEFT: lineEnd.x -= halfW; break;
                case Direction::RIGHT: lineEnd.x += halfW; break;
                default:break;
                }

                DrawLineEx(tilePosition, lineEnd, 1.0f ,GREEN);

                SpriteRenderData* tileRenderData = GetTileActiveRenderData(tile.type, tile.variantIndex);

                if(!tileRenderData)
                {
                    Color color = GetTileColor(tile.type);

                    if(IsColorOf(color, BLANK) || tile.type == TileType::PLATFORM_STOP) continue;

                    if(!tile.hitboxes.empty()) DrawRectangleRec(tileMainAABB, color);
                    else DrawRectangle(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE, color);
                }
            }
        }
    }

    for(int i = 0; i < platformCache_rendering.size(); i++)
    {
        Platform* platform = platformCache_rendering[i];

        Color platformColor = DECO;

        if(platform->type == PlatformType::MOVING_VERTICAL) platformColor = VERTICAL_MOVING_PLATFORM;
        else if(platform->type == PlatformType::MOVING_HORIZONTAL) platformColor = HORIZONTAL_MOVING_PLATFORM;
        else if(platform->type == PlatformType::FALLING) platformColor = FALLING_PLATFORM;
        else if(platform->type == PlatformType::DISAPPEARING) platformColor = DISAPPEARING_PLATFORM;

        if(!IsPlatformSpike(platform->type))
            DrawAABB(platform->gameObj.GetMainAABB(), platformColor);
        else
        {
            for(int h = 1; h < platform->gameObj.hitboxes.size(); h++)
            {
                DrawAABB(platform->gameObj.GetSubAABB(h), SPIKE, dynamicThickness);
            }
        }
    }

    for(int i = 0; i < enemyCache.size(); i++)
    {
        Enemy* enemy = enemyCache[i];

        DrawAABB(enemy->gameObj.GetMainAABB(), RED);

        DrawAABB(enemy->GetJumpDetector(), GREEN);
        DrawAABB(enemy->GetCeilingDetector(), BLUE);
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
            DrawAABB(room.aabb, RED, dynamicThickness);
        }
    }

    DrawAABB(player.gameObj.GetMainAABB(), ORANGE);

    DrawAABB(player.GetJumpDetector(), MAGENTA);
    DrawAABB(player.GetTreadmillDetector(), GREEN, 0.5f);
    DrawAABB(player.GetCeilingDetector(), RED);
    DrawAABB(player.GetLadderDetector(), SKYBLUE);
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