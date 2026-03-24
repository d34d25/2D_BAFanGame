#include "level.h"

Level::Level() : player({0, 0})
{
}

Level::~Level()
{
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            delete gameObjTiles[i][j];
        }
    }
}

void Level::InitLevel(const char *levelPath, float gridSize, float dt, int iterations)
{
    this->iterations = iterations;

    this->dt = dt;

    this->gridSize = gridSize;

    gravity = 200;

    player.entityData.flipY = gravity < 0;

    player.gravity = gravity;

    camera.target = {0,0};

    levelImage = LoadImage(levelPath);

    if(!IsImageValid(levelImage)) return;

    ClearGameObjMatrix();

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            Color pixelColor = GetImageColor(levelImage, i, j);

            bool isTile = IsColorOf(pixelColor, BLACK);

            bool isPlayerSpawn = IsColorOf(pixelColor, PLAYER_SPAWN);

            bool isGoal = IsColorOf(pixelColor, GOAL);
            
            bool isHorizontalPlatform = IsColorOf(pixelColor, HORIZONTAL_MOVING_PLATFORM);

            bool isVerticalPlatform =  IsColorOf(pixelColor, VERTICAL_MOVING_PLATFORM);

            bool isPlatformStop = IsColorOf(pixelColor, MOVING_PLATFORM_STOP);

            bool isTileTrampoline = IsColorOf(pixelColor, TRAMPOLINE);

            bool isGravityChanger = IsColorOf(pixelColor, GRAVITY_CHANGER);

            bool isTreadmillRight = IsColorOf(pixelColor, TREADMILL_RIGHT);

            bool isTreadmillLeft = IsColorOf(pixelColor, TREADMILL_LEFT);

            bool isFallingPlatform = IsColorOf(pixelColor, FALLING_PLATFORM);

            bool isOneWayUp = IsColorOf(pixelColor, ONE_WAY_UP);

            bool isOneWayDown = IsColorOf(pixelColor, ONE_WAY_DOWN);

            bool isOneWayRight = IsColorOf(pixelColor, ONE_WAY_RIGHT);

            bool isOneWayLeft = IsColorOf(pixelColor, ONE_WAY_LEFT);

            bool isDisappearingPlatform = IsColorOf(pixelColor, DISAPPEARING_PLATFORM);

            bool isSpike = IsColorOf(pixelColor, SPIKE);

            TileType type = TileType::VOID;

            if(isTile) type = TileType::SOLID;

            else if(isGoal) type = TileType::GOAL;

            else if(isPlatformStop)  type = TileType::PLATFORM_STOP;

            else if(isTileTrampoline) type = TileType::TRAMPOLINE;

            else if(isGravityChanger) type = TileType::GRAVITY_CHANGER;

            else if(isTreadmillRight) type = TileType::TREADMILL_RIGHT;

            else if(isTreadmillLeft) type = TileType::TREADMILL_LEFT;

            else if(isOneWayUp) type = TileType::ONE_WAY_UP;

            else if(isOneWayDown) type = TileType::ONE_WAY_DOWN;

            else if(isOneWayRight) type = TileType::ONE_WAY_RIGHT;

            else if(isOneWayLeft) type = TileType::ONE_WAY_LEFT;

            else if(isSpike) type = TileType::SPIKE;

            level[i][j].type = type;

            if(isPlayerSpawn)
            {
                float xpos = i * gridSize + gridSize * 0.5f;
                float ypos = j * gridSize + gridSize * 0.5f;

                player.phys.position = {xpos, ypos};
            }

            //platforms...
        }
    }

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {

            Tile* tile = &level[i][j];
            TileType type = tile->type;

            if(type == TileType::VOID) continue;

            float xpos = i * gridSize + gridSize * 0.5f;
            float ypos = j * gridSize + gridSize * 0.5f;

            gameObjTiles[i][j] = new GameObject();

            GameObject* objTile = gameObjTiles[i][j];

            objTile->aabb = {0, 0, gridSize, gridSize};

            objTile->position = {xpos, ypos};

            objTile->UpdateAABB();

            float treadmillVel = 10.0f;

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

                objTile->velocity.x = treadmillVel;
            }
            break;

            case TileType::TREADMILL_LEFT:
            {
                objTile->canEntityCollide = true;
                objTile->canPlatformCollide = true;

                objTile->velocity.x = -treadmillVel;
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
    player.Update(dt, iterations);

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

    player.phys.UpdatePositionX(dt);

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = gameObjTiles[i][j];

            if(!objTile) continue;

            if(!objTile->canEntityCollide) continue;

            SolveCollisions(&player.phys, objTile, true, isGravityUp);
        }
    }


    player.phys.UpdatePositionY(dt);

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            GameObject* objTile = gameObjTiles[i][j];

            if(!objTile) continue;

            if(!objTile->canEntityCollide) continue;

            SolveCollisions(&player.phys, objTile, false, isGravityUp);
        }
    }
}

void Level::DrawLevel()
{
    BeginMode2D(camera);

    TileRange playerTileRange = CalculateTileRange(
        player.phys.position.x,
        player.phys.position.y,
        14
    );

    for(int i = playerTileRange.startX; i <= playerTileRange.endX; i++)
    {
        for(int j = playerTileRange.startY; j <= playerTileRange.endY; j++)
        {
            Color color = {0,0,0,0};

            TileType type = level[i][j].type;

            if(type == TileType::VOID) continue;

            switch (type)
            {
            case TileType::SOLID:
                color = BLACK;
                break;

            case TileType::GOAL:
                color = GOAL;
                break;

            case TileType::TRAMPOLINE:
                color = TRAMPOLINE;
                break;

            case TileType::TREADMILL_RIGHT:
                color = TREADMILL_RIGHT;
                break;
            
            case TileType::TREADMILL_LEFT:
                color = TREADMILL_LEFT;
                break;

            case TileType::ONE_WAY_UP:
                color = ONE_WAY_UP;
                break;

            case TileType::ONE_WAY_DOWN:
                color = ONE_WAY_DOWN;
                break;

            case TileType::ONE_WAY_RIGHT:
                color = ONE_WAY_RIGHT;
                break;

            case TileType::ONE_WAY_LEFT:
                color = ONE_WAY_LEFT;
                break;

            case TileType::SPIKE:
                color = SPIKE;
                break;

            case TileType::GRAVITY_CHANGER:
                color = GRAVITY_CHANGER;
                break;

            default:
                break;
            }

            if(IsColorOf(color, Color{0,0,0,0})) continue;

            else if(gameObjTiles[i][j]) DrawRectangleRec(gameObjTiles[i][j]->aabb, color);
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
    DrawText(TextFormat("Player X speed: %.4f", player.phys.velocity.x), 10, 100, 20, GRAY);
    DrawText(TextFormat("Player Y speed: %.4f", player.phys.velocity.y), 10, 120, 20, GRAY);
}