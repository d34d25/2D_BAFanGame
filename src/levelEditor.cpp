#include "levelEditor.h"

#include<iostream>
#include<cstring>

void LevelEditor::ExportLevel()
{
    int dataSize = LAYERS * ROWS * COLS * sizeof(Tile);

    if(SaveFileData("testLevel", tempLevel, dataSize))
    {
        std::cout<<"LEVEL EXPORT SUCCEED"<<"\n";
    }
    else 
    {
        std::cout<<"LEVEL EXPORT FAILED"<<"\n";
    }

    int roomCount = rooms.size();
    int roomDataSize = sizeof(Room) * roomCount;

    if(SaveFileData("testRooms", rooms.data(), roomDataSize))
    {
        std::cout<<"ROOMS EXPORT SUCCEED"<<"\n";
    }
    else
    {
        std::cout<<"ROOMS EXPORT FAILED"<<"\n";
    }
}

void LevelEditor::DrawRotatingSpikesRec(int currentType, Vector2 position, float size, EntityData data, Color color)
{
    switch (currentType)
    {
    case (int)TileType::ROTATING_SPIKE_SINGLE:
    {
        for(int i = 0; i < SINGLE_ROTATING_SPIKE_MAX_HITBOX; i++)
        {
            Vector2 currentOffset = {0,0};

            if(i > 0)
            {
                float multiplier = (float)i * size;

                currentOffset.x = data.flipX ? -multiplier : multiplier;
                currentOffset.y = data.flipY ? multiplier : -multiplier;
            }

            DrawRectangle(
                position.x - (size * 0.5f) + currentOffset.x, 
                position.y - (size * 0.5f) + currentOffset.y,
                size, size, 
                color
            );
        }
    }
    break;

    case (int)TileType::ROTATING_SPIKE_DOUBLE:
    {
        for(int i = 0; i < SINGLE_ROTATING_SPIKE_MAX_HITBOX * 2; i++)
        {
            if(i == SINGLE_ROTATING_SPIKE_MAX_HITBOX) continue;

            float size = GRID_SIZE * 0.5f;

            Vector2 currentOffset = {0,0};

            float armSide = (i < SINGLE_ROTATING_SPIKE_MAX_HITBOX) ? 1.0f : -1.0f;

            int localI = i % SINGLE_ROTATING_SPIKE_MAX_HITBOX;

            if(localI > 0)
            {
                float multiplier = (float)localI * size * armSide;

                currentOffset.x = data.flipX ? -multiplier : multiplier;
                currentOffset.y = data.flipY ? multiplier : -multiplier;
            }

            DrawRectangle(
                position.x - (size * 0.5f) + currentOffset.x, 
                position.y - (size * 0.5f) + currentOffset.y,
                size, size, 
                color
            );
        }
    }
    break;
    
    default:
        break;
    }
}

void LevelEditor::DrawRotatingSpikesSprite(int currentType, int frame, const SpriteRenderData& renderData, Vector2 position, float size, const EntityData& data)
{
    switch (currentType)
    {
    
    case (int)TileType::ROTATING_SPIKE_DOUBLE:
    {
        for(int i = 0; i < SINGLE_ROTATING_SPIKE_MAX_HITBOX * 2; i++)
        {
            if(i == SINGLE_ROTATING_SPIKE_MAX_HITBOX) continue;

            float size = GRID_SIZE * 0.5f;

            Vector2 currentOffset = {0,0};

            float armSide = (i < SINGLE_ROTATING_SPIKE_MAX_HITBOX) ? 1.0f : -1.0f;

            int localI = i % SINGLE_ROTATING_SPIKE_MAX_HITBOX;

            if(localI > 0)
            {
                float multiplier = (float)localI * size * armSide;

                currentOffset.x = data.flipX ? -multiplier : multiplier;
                currentOffset.y = data.flipY ? multiplier : -multiplier;
            }

            float xPos = position.x - (size * 0.5f) + currentOffset.x;
            float yPos = position.y - (size * 0.5f) + currentOffset.y;

            DrawSprite(&renderData, xPos, yPos, frame, data.flipX, data.flipY, WHITE);
        }
    }
    break;

    case (int)TileType::ROTATING_SPIKE_SINGLE:
    {
        for(int i = 0; i < SINGLE_ROTATING_SPIKE_MAX_HITBOX; i++)
        {
            Vector2 currentOffset = {0,0};

            if(i > 0)
            {
                float multiplier = (float)i * size;

                currentOffset.x = data.flipX ? -multiplier : multiplier;
                currentOffset.y = data.flipY ? multiplier : -multiplier;
            }

            float xPos = position.x - (size * 0.5f) + currentOffset.x;
            float yPos = position.y - (size * 0.5f) + currentOffset.y;

            DrawSprite(&renderData, xPos, yPos, frame, data.flipX, data.flipY, WHITE);
        }
    }
    break;
    
    default: break;
    }
}

LevelEditor::LevelEditor(int screenWidth, int screenHeight, const char* levelPath, const char* roomPath)
{
    this->levelPath = levelPath;

    this->roomPath = roomPath;

    camera.zoom = 1.0f;

    camera.offset = {screenWidth * 0.5f, screenHeight * 0.5f};

    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    int worldWidth = COLS * GRID_SIZE;
    int worldHeight = ROWS * GRID_SIZE;

    int halfWorldWidth = (int)floor(worldWidth * 0.5f);
    int halfWorldHeight = (int)floor(worldHeight * 0.5f);

    camera.target = {(float)halfWorldWidth, (float)halfWorldHeight};

    ResetRooms();

    currentTexture = 0;
}

LevelEditor::~LevelEditor()
{
}

void LevelEditor::Update()
{
    UpdateCamera();

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    int i = (int)floor(mouseWorldPos.x / GRID_SIZE);
    int j = (int)floor(mouseWorldPos.y / GRID_SIZE);

    i = Clamp(i, 0, COLS - 1);
    j = Clamp(j, 0, ROWS - 1);

    mouseMatrixPosition = {i,j};

    activeRenderData = GetTileActiveRenderData((TileType)currentTileType, currentVariant);

    std::vector<SpriteRenderData>* activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

    if(!activeRenderDataList) currentVariant = 0;

    float mouseWheel = GetMouseWheelMove();

    if(IsKeyPressed(KEY_TAB))
    {
        currentLayer++;

        if(currentLayer >= LAYERS) currentLayer = 0;
    }

    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_C))
    {
        for(int l = 0; l < LAYERS; l++)
        {
            for(int i = 0; i < COLS; i++)
            {
                for(int j = 0; j < ROWS; j++)
                {
                    tempLevel[l][i][j] = {};
                }
            }
        }

        ResetRooms();
    }

    if(IsKeyDown(KEY_LEFT_ALT) && mouseWheel != 0)
    {
        float cameraZoomFactor = 0.25f;

        if(mouseWheel > 0) camera.zoom += cameraZoomFactor;
        else if(mouseWheel < 0) camera.zoom -= cameraZoomFactor;

        camera.zoom = Clamp(camera.zoom, 0.1f,10.0f);
    }

    if(IsKeyPressed(KEY_R)) roomMode = !roomMode;

    if(!environmentPalettes.empty())
        {
        if(IsKeyPressed(KEY_UP))
        {
            currentBackgroundPalette++;

            if(currentBackgroundPalette >= environmentPalettes.size()) currentBackgroundPalette = 0;
        }
        else if(IsKeyPressed(KEY_DOWN))
        {
            currentBackgroundPalette--;

            if(currentBackgroundPalette < 0) currentBackgroundPalette = environmentPalettes.size() - 1;
        }

        if(IsKeyPressed(KEY_RIGHT))
        {
            currentBackgroundColor++;

            if(currentBackgroundColor >= environmentPalettes.at(currentBackgroundPalette).size()) currentBackgroundColor = 0;
        }
        else if(IsKeyPressed(KEY_LEFT))
        {
            currentBackgroundColor--;

            if(currentBackgroundColor < 0) currentBackgroundColor = environmentPalettes.at(currentBackgroundPalette).size() - 1;
        }
    }

    if(!roomMode)
    {
        if(IsNumKeyPressed())
        {
            if(IsKeyPressed(KEY_ZERO)) currentTileType = (int)TileType::VOID;

            if(IsKeyPressed(KEY_ONE)) currentTileType = (int)TileType::TILE_START + 1;

            if(IsKeyPressed(KEY_TWO)) currentTileType = (int)TileType::SPIKE_START + 1;

            if(IsKeyPressed(KEY_THREE)) currentTileType = (int)TileType::PLATFORM_START + 1;

            if(IsKeyPressed(KEY_FOUR)) currentTileType = (int)TileType::MISC_START + 1;

            if(IsKeyPressed(KEY_FIVE)) currentTileType = (int)TileType::DECO;

            if(IsKeyPressed(KEY_SIX)) currentTileType = (int)TileType::ENEMY_START + 1;

            activeRenderData = GetTileActiveRenderData((TileType)currentTileType, 0);

            activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

            currentVariant = 0;

            currentTexture = 0;

            currentAngle = 0;

            currentData = {false, false};

            currentDirection = Direction::UP;

            chunkSize = 1;
        }

        if(IsDirectionChangeKeyPressed())
        {
            if(IsKeyPressed(KEY_E))
            {
                currentAngle += 90;

                if(currentAngle >= 360) currentAngle = 0;
            }

            if(IsKeyPressed(KEY_Q))
            {
                currentAngle -= 90;

                if(currentAngle < 0) currentAngle = 270;
            }

            if(IsKeyPressed(KEY_H)) currentData.flipX = !currentData.flipX;

            if(IsKeyPressed(KEY_V)) currentData.flipY = !currentData.flipY;

            currentDirection = CalculateDirection(currentAngle, currentData);
        }

        if(IsKeyPressed(KEY_X) && currentTileType == (int)TileType::SOLID)
        {
            chunkSize++;

            if(chunkSize > maxChunkSize) chunkSize = 1;
            else if(chunkSize < 1) chunkSize = maxChunkSize;
        }
        else if(currentTileType != (int)TileType::SOLID)
        {
            chunkSize = 1;
        }

        std::vector<std::array<Color, MAX_PALETTE_COLS>>* activePaletteList = GetCurrentTilePaletteList((TileType)currentTileType);

        if(!activePaletteList->empty())
        {
            if(IsKeyPressed(KEY_W))
            {
                currentPalette++;

                if(currentPalette >= activePaletteList->size()) currentPalette = 0;
            }
            else if(IsKeyPressed(KEY_S))
            {
                currentPalette--;

                if(currentPalette < 0) currentPalette = activePaletteList->size() - 1;
            }

            currentPaletteColors = activePaletteList->at(currentPalette);
        }

        //variant cycling
        if(activeRenderDataList && !activeRenderDataList->empty())
        {
            if(mouseWheel != 0 && IsKeyDown(KEY_SPACE))
            {
                std::vector<SpriteRenderData>* activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

                if(activeRenderDataList)
                {
                    int direction = (mouseWheel > 0) ? 1 : -1;

                    currentVariant += direction;

                    if(currentVariant < 0)currentVariant = activeRenderDataList->size() - 1;
                    else if(currentVariant >= activeRenderDataList->size()) currentVariant = 0;
                }
            }
        }
        else
        {
            currentVariant = 0;
        }

        bool reactiveAnimations = HasReactiveAnimations((TileType)currentTileType);

        //texture cycling
        if(mouseWheel != 0 && IsKeyDown(KEY_LEFT_SHIFT) && !reactiveAnimations)
        {
            if(activeRenderData && !activeRenderData->animationFrames.empty())
            {
                int direction = (mouseWheel > 0) ? 1 : -1;

                currentTexture += activeRenderData->spacing * direction;

                int frameCount = (int)activeRenderData->animationFrames.size();

                std::cout<<"currentTexture: "<<currentTexture<<"\n";

                if(currentTexture < 0)
                    currentTexture = frameCount - activeRenderData->spacing;
                else if(currentTexture >= frameCount)
                    currentTexture = 0;
            }
            else
            {
                currentTexture = 0;
            }
        }
        else if(reactiveAnimations)
        {
            currentTexture = 0;
        }

        //tile type cycling
        if(mouseWheel != 0 && !IsKeyDown(KEY_LEFT_ALT) && !IsKeyDown(KEY_LEFT_SHIFT) && !IsKeyDown(KEY_SPACE))
        {
            int direction = (mouseWheel > 0) ? 1 : -1;

            int start = (int)TileType::VOID;

            int end = (int)TileType::COUNT;

            if(currentTileType >= (int)TileType::TILE_START && currentTileType <= (int)TileType::TILE_END)
            {
                start = (int)TileType::TILE_START;
                end = (int)TileType::TILE_END;
            }
            else if(currentTileType >= (int)TileType::SPIKE_START && currentTileType <= (int)TileType::SPIKE_END)
            {
                start = (int)TileType::SPIKE_START;
                end = (int)TileType::SPIKE_END;
            }
            else if(currentTileType >= (int)TileType::PLATFORM_START && currentTileType <= (int)TileType::PLATFORM_END)
            {
                start = (int)TileType::PLATFORM_START;
                end = (int)TileType::PLATFORM_END;
            }
            else if(currentTileType >= (int)TileType::MISC_START && currentTileType <= (int)TileType::MISC_END)
            {
                start = (int)TileType::MISC_START;
                end = (int)TileType::MISC_END;
            }
            else if(currentTileType >=(int)TileType::ENEMY_START && currentTileType <= (int)TileType::ENEMY_END)
            {
                start = (int)TileType::ENEMY_START;
                end = (int)TileType::ENEMY_END;
            }
            else
            {
                direction = 0;
            }

            currentTileType += direction;

            if(direction != 0)
            {
                currentAngle = 0;

                currentData = {false, false};

                currentDirection = Direction::UP;
            }

            while(IsTypeInvalid((TileType)currentTileType))
            {
                if(currentTileType >= end) currentTileType = start + 1;
                else if(currentTileType <= start) currentTileType = end - 1;
                else currentTileType += direction;
            }

            activeRenderData = GetTileActiveRenderData((TileType)currentTileType, currentVariant);

            if(!activeRenderData || activeRenderData->animationFrames.empty())
            {
                currentTexture = 0;
                currentVariant = 0;
            }
            else currentTexture = 0;
        }

        Tile& targetTile = tempLevel[currentLayer][mouseMatrixPosition.x][mouseMatrixPosition.y];

        bool isDecorationLayer = (currentLayer == BACKGROUND_LAYER || currentLayer == FOREGROUND_LAYER);

        bool canPlaceTile = false;

        //prevents static tiles from flipping their offsets
        //only the player, platforms and enemies are allowed to flip their offsets (dynamic objects)
        if(currentTileType == (int)TileType::PLAYER_SPAWN ||
            currentTileType > (int)TileType::ENEMY_START && currentTileType < (int)TileType::ENEMY_END ||
            currentTileType > (int)TileType::PLATFORM_START && currentTileType < (int)TileType::PLATFORM_END
        )
        {
            currentData.flipOffset = true;
        }
        else
        {
            currentData.flipOffset = false;
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if(currentTileType != (int)TileType::VOID
                && currentTileType != (int)targetTile.type
                && !IsTypeInvalid((TileType)currentTileType)
            )
            {
                TileType placingType = (TileType)currentTileType;

                if(!isDecorationLayer)
                {
                    canPlaceTile = true;
                }
                else
                {
                    if(placingType == TileType::DECO)
                    {
                        canPlaceTile = true;
                    }
                    else
                    {
                        std::cout<<"CAN'T PLACE GAMEPLAY ELEMENTS IN THIS LAYER \n";
                    }
                }

                if(canPlaceTile)
                {
                    if((TileType)currentTileType == TileType::PLAYER_SPAWN)
                    {
                        for(int l = 0; l < LAYERS; l++)
                        {
                            for(int i = 0; i < COLS; i++)
                            {
                                for(int j = 0; j < ROWS; j++)
                                {
                                    if(tempLevel[l][i][j].type == TileType::PLAYER_SPAWN)
                                    {
                                        tempLevel[l][i][j] = {};
                                    }
                                }
                            }
                        }   
                    }

                    bool isFrameSmallerThanTile = true;

                    if(activeRenderData)
                    {
                        isFrameSmallerThanTile = activeRenderData->frameSize.x <= TILE_SIZE && activeRenderData->frameSize.y <= TILE_SIZE;
                    }
                    
                    bool canPlaceChunk = false;

                    if(chunkSize > 1 && Vector2Equals(activeRenderData->frameSize, {(float)TILE_SIZE * chunkSize, (float)TILE_SIZE * chunkSize}))
                    {
                        canPlaceChunk = true;
                    }
                    else if(isFrameSmallerThanTile)
                    {
                        canPlaceChunk = true;
                    }

                    if(currentTileType != (int)TileType::SOLID) canPlaceChunk = true;

                    if(currentTileType != (int)TileType::SOLID && chunkSize > 1) canPlaceChunk = false;

                    if(canPlaceChunk)
                    {
                        for(int x = 0; x < chunkSize; x++)
                        {
                            for(int y = 0; y < chunkSize; y++)
                            {
                                int endMatrixX = mouseMatrixPosition.x + x;
                                int endMatrixY = mouseMatrixPosition.y + y;
                                    
                                endMatrixX = Clamp(endMatrixX, 0, COLS);
                                endMatrixY = Clamp(endMatrixY, 0, ROWS);

                                Tile& targetSubTile = tempLevel[currentLayer][endMatrixX][endMatrixY];

                                if(targetSubTile.type != TileType::VOID) continue;

                                Vector2 endPos = {(endMatrixX * GRID_SIZE) + GRID_SIZE * 0.5f, (endMatrixY * GRID_SIZE) + GRID_SIZE * 0.5f};

                                bool canPlaceTexture = false;

                                if(isFrameSmallerThanTile)
                                {
                                    targetSubTile.textureIndex = currentTexture;
                                }
                                else
                                {
                                    if(x == 0 && y == 0) targetSubTile.textureIndex = currentTexture;
                                    else targetSubTile.textureIndex = -1;
                                }

                                targetSubTile.type = placingType;

                                targetSubTile.variantIndex = currentVariant;

                                targetSubTile.gameObj.transform.position = endPos;

                                targetSubTile.gameObj.transform.scale = TILE_SCALE;

                                targetSubTile.gameObj.transform.angle = currentAngle;

                                targetSubTile.gameObj.data = currentData;

                                targetSubTile.gameObj.direction = currentDirection;

                                targetSubTile.paletteIndex = currentPalette;
                            }
                        }
                    }   
                }
                else
                {
                    std::cout<<"SOLID TILE FRAME SIZE DOES NOT MATCH WITH THE SIZE OF THE CHUNK\n";
                }
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            for(int x = 0; x < chunkSize; x++)
            {
                for(int y = 0; y < chunkSize; y++)
                {
                    int endMatrixX = mouseMatrixPosition.x + x;
                    int endMatrixY = mouseMatrixPosition.y + y;
                                    
                    endMatrixX = Clamp(endMatrixX, 0, COLS);
                    endMatrixY = Clamp(endMatrixY, 0, ROWS);

                    Tile& targetSubTile = tempLevel[currentLayer][endMatrixX][endMatrixY];

                    if(targetSubTile.type != TileType::VOID)
                    {
                        targetSubTile = {};
                    }
                }
            }
        }

        if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_CONTROL))
        {
            for(int l = 0; l < LAYERS; l++)
            {
                for(int i = 0; i < COLS; i++)
                {
                    for(int j = 0; j < ROWS; j++)
                    {
                        tempLevel[l][i][j] = {};
                    }
                }
            }
        }
    }
    else
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        int hoveredRoomIndex = -1;

        for(int i = 0; i < rooms.size(); i++)
        {
            if(CheckCollisionPointRec(mouseWorldPos, rooms[i].aabb))
            {
                hoveredRoomIndex = i;
                break;
            }
        }

        if(hoveredRoomIndex > -1 && hoveredRoomIndex < rooms.size())
        {
            Rectangle currentAABB = rooms[hoveredRoomIndex].aabb;

            int threshold = GRID_SIZE * 2;

            //this starts at the center of the current room
            targetNeigbourPos = {
                currentAABB.x + currentAABB.width * 0.5f,
                currentAABB.y + currentAABB.height * 0.5f
            };

            int offset = GRID_SIZE;

            Vector2 displacement = {
                currentAABB.width * 0.5f + offset,
                currentAABB.height * 0.5f + offset
            };

            if(mouseWorldPos.x < currentAABB.x + threshold)
            {
                targetNeigbourPos.x -= displacement.x;
            }
            else if(mouseWorldPos.x > (currentAABB.x + currentAABB.width) - threshold)
            {
                targetNeigbourPos.x += displacement.x;
            }
            else if(mouseWorldPos.y < currentAABB.y + threshold)
            {
                targetNeigbourPos.y -= displacement.y;
            }
            else if(mouseWorldPos.y > (currentAABB.y + currentAABB.height) - threshold)
            {
                targetNeigbourPos.y += displacement.y;
            }

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int neighbourIndex = -1;

                for(int i = 0; i < rooms.size(); i++)
                {
                    if(i != hoveredRoomIndex && CheckCollisionPointRec(targetNeigbourPos, rooms[i].aabb))
                    {
                        neighbourIndex = i;
                        break;
                    }
                }

                if(neighbourIndex > -1 && neighbourIndex < rooms.size())
                {
                    Rectangle neighborAABB = rooms[neighbourIndex].aabb;

                    bool validHorizontal = (currentAABB.y == neighborAABB.y && currentAABB.height == neighborAABB.height);
                    bool validVertical = (currentAABB.x == neighborAABB.x && currentAABB.width == neighborAABB.width);

                    if(validHorizontal || validVertical)
                    {
                        Vector2 min = {
                            std::min(currentAABB.x, neighborAABB.x),
                            std::min(currentAABB.y, neighborAABB.y)
                        };

                        Vector2 max = {
                            std::max(currentAABB.x + currentAABB.width, neighborAABB.x + neighborAABB.width),
                            std::max(currentAABB.y + currentAABB.height, neighborAABB.y + neighborAABB.height)
                        };

                        rooms[hoveredRoomIndex].aabb = {min.x, min.y, max.x - min.x, max.y - min.y};

                        rooms.erase(rooms.begin() + neighbourIndex);
                    }
                }
            }

            if(IsKeyPressed(KEY_B))
            {
                rooms[hoveredRoomIndex].currentPaletteIndex = currentBackgroundPalette;
                rooms[hoveredRoomIndex].currentColorIndex = currentBackgroundColor;
            }

            if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            {
                Rectangle roomToSplit = rooms[hoveredRoomIndex].aabb;

                float roomWidth = (float)(TILES_PER_ROOM_WIDHT * GRID_SIZE);
                float roomHeight = (float)(TILES_PER_ROOM_HEIGHT * GRID_SIZE);

                int colsInRoom = (int)roundf(roomToSplit.width / roomWidth);
                int rowsInRoom = (int)roundf(roomToSplit.height / roomHeight);

                rooms.erase(rooms.begin() + hoveredRoomIndex);

                for(int i = 0; i < rowsInRoom; i++)
                {
                    for(int j = 0; j < colsInRoom; j++)
                    {
                        Room unitRoom;

                        unitRoom.aabb = {
                            roomToSplit.x + (j * roomWidth),
                            roomToSplit.y + (i * roomHeight),
                            roomWidth,
                            roomHeight
                        };

                        rooms.push_back(unitRoom);
                    }
                }
            }
        }
    }
   
    if(IsKeyPressed(KEY_F5))
    {
        ExportLevel();
    }
    else if(IsKeyPressed(KEY_F9))
    {
        LoadLevelData(levelPath, tempLevel, roomPath, rooms);
    }
}

void LevelEditor::Draw()
{
    int worldWidth = COLS * GRID_SIZE;
    int worldHeight = ROWS * GRID_SIZE;

    int halfWorldWidth = (int)floor(worldWidth * 0.5f);
    int halfWorldHeight = (int)floor(worldHeight * 0.5f);

    float lineThickness = 1.5f;
    float dynamicThickness = lineThickness / camera.zoom;

    Color backgroundColor = environmentPalettes.at(currentBackgroundPalette).at(currentBackgroundColor);

    BeginMode2D(camera);

    TileRange cameraTileRange = CalculateTileRange(
        camera.target.x, camera.target.y, 100
    );

    lastPaletteIndex = -1;
    lastPaletteList = nullptr;

    ClearBackground(GRAY);

    for(int i = 0; i < rooms.size(); i++)
    {
        DrawRectangleRec(rooms[i].aabb, environmentPalettes.at(rooms[i].currentPaletteIndex).at(rooms[i].currentColorIndex));
    }

    BeginShaderMode(paletteShader);

    for(int l = 0; l < LAYERS; l++)
    {
        for(int i = cameraTileRange.startX; i <= cameraTileRange.endX; i++)
        {
            for(int j = cameraTileRange.startY; j <= cameraTileRange.endY; j++)
            {
                bool isLayerActive = (l == currentLayer);

                unsigned char alpha = isLayerActive ? 255 : 80;

                Color layerTint = {255,255,255,alpha};

                Tile tile = tempLevel[l][i][j];

                TileType type = tile.type;

                if(type == TileType::VOID || type >= TileType::COUNT) continue;

                Color color = GetTileColor(type);

                SpriteRenderData* tileRenderData = GetTileActiveRenderData(type, tile.variantIndex);

                Vector2 tileSize = {GRID_SIZE, GRID_SIZE};

                float offsetX = 0;
                float offsetY = 0;

                switch (type)
                {
                case TileType::VERTICAL_MOVING_PLATFORM:
                case TileType::HORIZONALT_MOVING_PLATFORM:
                {
                    tileSize.x = GRID_SIZE * 3.0f;
                    tileSize.y = GRID_SIZE * 0.3f;

                    offsetX = -GRID_SIZE;
                    offsetY = tileSize.y;
                }
                break;

                default: break;
                }

                if(tile.textureIndex < 0 || !tileRenderData)
                {
                    rlDrawRenderBatchActive();

                    if(tile.type == TileType::ROTATING_SPIKE_SINGLE ||
                    tile.type == TileType::ROTATING_SPIKE_DOUBLE)
                    {
                        DrawRotatingSpikesRec((int)tile.type, tile.gameObj.transform.position, GRID_SIZE * 0.5f, tile.gameObj.data, {color.r, color.g, color.b, alpha});
                    }
                    else
                    {
                        Vector2 size = {tileSize.x, tileSize.y};

                        float playerOffsetY = 0.0f;

                        if(tile.type == TileType::PLAYER_SPAWN)
                        {
                            size.y = tileSize.y * 2.0f;

                            playerOffsetY = tileSize.y;
                        }

                        Rectangle placeHolderRec = {i * GRID_SIZE + offsetX, j * GRID_SIZE + offsetY - playerOffsetY, size.x, size.y};

                        if(tile.type == TileType::SOLID)
                        {
                            DrawRectangleLinesEx(
                                placeHolderRec, dynamicThickness + dynamicThickness * 0.15f,
                                {color.r, color.g, color.b, alpha}
                            );
                        }
                        else
                        {
                            DrawRectangleRec(placeHolderRec, {color.r, color.g, color.b, alpha});
                        }                        
                    }
                }
                else
                {
                    std::vector<std::array<Color, MAX_PALETTE_COLS>>* currentTilePaletteList = GetCurrentTilePaletteList(type);

                    ChangePalette(tile.paletteIndex, currentTilePaletteList);

                    if(tile.type == TileType::ROTATING_SPIKE_SINGLE ||
                    tile.type == TileType::ROTATING_SPIKE_DOUBLE)
                    {
                        DrawRotatingSpikesSprite(
                            (int)tile.type,
                            tile.textureIndex,
                            *tileRenderData,
                            tile.gameObj.transform.position,
                            GRID_SIZE * 0.5f,
                            tile.gameObj.data
                        );
                    }
                    else
                    {
                        DrawSprite(tile.gameObj, tileRenderData, tile.textureIndex, layerTint);
                    }
                }
            }
        }
    }

    if(!roomMode)
    {
        //preview
        Color color = GetTileColor((TileType)currentTileType);

        Color previewColor = color;

        if(currentTileType != (int)TileType::VOID) previewColor.a = 50;

        Vector2 tileSize = {GRID_SIZE, GRID_SIZE};
        
        Transform2D previewTransform;

        previewTransform.position = GetMouseGridPosition(mouseMatrixPosition);
        previewTransform.scale = TILE_SCALE;
        previewTransform.angle = currentAngle;

        float offsetX = 0;
        float offsetY = 0;

        switch ((TileType)currentTileType)
        {
        case TileType::VERTICAL_MOVING_PLATFORM:
        case TileType::HORIZONALT_MOVING_PLATFORM:
        {
            tileSize.x = GRID_SIZE * 3.0f;
            tileSize.y = GRID_SIZE * 0.3f;

            offsetX = -GRID_SIZE;
            offsetY = tileSize.y;
        }
        break;

        default: break;
        }

        if(currentTexture < 0 || !activeRenderData)
        {
            rlDrawRenderBatchActive();

            if(currentTileType == (int)TileType::ROTATING_SPIKE_SINGLE ||
            currentTileType == (int)TileType::ROTATING_SPIKE_DOUBLE)
            {
                DrawRotatingSpikesRec(currentTileType, previewTransform.position, GRID_SIZE * 0.5f, currentData, previewColor);
            }
            else
            {
                Vector2 size = {tileSize.x, tileSize.y};

                float playerOffsetY = 0.0f;

                if(currentTileType == (int)TileType::PLAYER_SPAWN)
                {
                    size.y = tileSize.y * 2.0f;

                    playerOffsetY = tileSize.y;
                }

                DrawRectangle(
                    previewTransform.position.x - (GRID_SIZE * 0.5f) + offsetX, 
                    previewTransform.position.y - (GRID_SIZE * 0.5f) + offsetY - playerOffsetY,
                    size.x * chunkSize, size.y * chunkSize,
                    previewColor
                );
            }

            lastPaletteIndex = -1;
            lastPaletteList = nullptr;
        }
        else
        {
            rlDrawRenderBatchActive();

            previewColor = WHITE;
            previewColor.a = 100;

            SetShaderPalette(paletteShader, paletteLoc, currentPaletteColors);

            if(currentTileType == (int)TileType::ROTATING_SPIKE_SINGLE ||
                currentTileType == (int)TileType::ROTATING_SPIKE_DOUBLE)
            
            {
                DrawRotatingSpikesSprite(
                    currentTileType,
                    currentTexture,
                    *activeRenderData,
                    previewTransform.position,
                    GRID_SIZE * 0.5f,
                    currentData
                );
            }
            else
            {
                DrawSprite(previewTransform, activeRenderData, currentData, currentTexture, previewColor);
            }
            
            rlDrawRenderBatchActive();
        }
    }

    EndShaderMode();

    if(roomMode) DrawCircle(targetNeigbourPos.x, targetNeigbourPos.y, 20, RED);
    
    for(int i = 0; i < rooms.size(); i++)
    {
        DrawAABB(rooms[i].aabb, PURPLE, 6.0f);
    }

    //grid

    for(int i = 0; i <= worldWidth; i+= GRID_SIZE)
    {
        DrawLineEx(
            {(float)i, 0.0f},
            {(float)i, (float)worldHeight},
            dynamicThickness,
            GRAY
        );
    }

    for(int i = 0; i <= worldHeight; i+= GRID_SIZE)
    {
        DrawLineEx(
            {0.0f, (float)i},
            {(float)worldWidth, (float)i},
            dynamicThickness,
            GRAY
        );
    }

    DrawRectangleLines(
        mouseMatrixPosition.x * GRID_SIZE, 
        mouseMatrixPosition.y * GRID_SIZE,
        GRID_SIZE * chunkSize, GRID_SIZE * chunkSize, RED
    );


    EndMode2D();
    
    //menu

    int ypos = 50;
    int spacing = 30;

    DrawText(TextFormat("mouse x: %i", mouseMatrixPosition.x), GRID_SIZE, ypos, 20, WHITE);

    DrawText(TextFormat("mouse y: %i", mouseMatrixPosition.y), GRID_SIZE, ypos + spacing, 20, WHITE);

    DrawText(GetTileTypeText((TileType)currentTileType), GRID_SIZE, ypos + spacing * 2, 20, WHITE);
    
    DrawText(TextFormat("tileType: %i", currentTileType ), GRID_SIZE, ypos + spacing * 3,20,WHITE);

    const char* layerText = "";

    switch (currentLayer)
    {
    case BACKGROUND_LAYER: layerText = "BACKGROUND"; break;

    case FOREGROUND_LAYER: layerText = "FOREGROUND"; break;

    case GAMEPLAY_LAYER_START: layerText = "GAMEPLAY 1"; break;

    case GAMEPLAY_LAYER_END: layerText = "GAMEPLAY 2"; break;
    
    default: break;
    }

    DrawRectangle(0,0, GRID_SIZE, GRID_SIZE, backgroundColor);

    DrawText(TextFormat("current layer: %s", layerText), GRID_SIZE, ypos + spacing * 4, 20, RED);

    DrawText(TextFormat("current angle: %i", currentAngle), GRID_SIZE, ypos + spacing * 5, 20, RED);

    DrawText(TextFormat("current flip x: %i", currentData.flipX), GRID_SIZE, ypos + spacing * 6, 20, RED);

    DrawText(TextFormat("current flip y: %i", currentData.flipY), GRID_SIZE, ypos + spacing * 7, 20, RED);

    DrawText(GetDirectionText(currentDirection), GRID_SIZE, ypos + spacing * 8, 20, RED);

    DrawText(TextFormat("room mode: %i", roomMode), GRID_SIZE, ypos + spacing * 9, 20, GREEN);

    DrawText(TextFormat("current palette: %i", currentPalette), GRID_SIZE, ypos + spacing * 10, 20, GREEN);

    DrawText(TextFormat("current chunk size: %i", chunkSize), GRID_SIZE, ypos + spacing * 11, 20, GREEN);

    int size = 20;
    int offsetY = size;

    for(int y = 0; y < spritePalettes.size(); y++)
    {
        for(int x = 0; x < MAX_PALETTE_COLS; x++)
        {
            Color colorToDraw = spritePalettes[y][x];

            int offsetX = GetScreenWidth() - (int)(environmentPalettes.size() * size * 1.25f);

            DrawRectangle(offsetX + (x * size), offsetY + (y * size), size, size, colorToDraw);
        }
    }

    for(int y = 0; y < environmentPalettes.size(); y++)
    {
        for(int x = 0; x < MAX_PALETTE_COLS; x++)
        {
            Color colorToDraw = environmentPalettes[y][x];

            int offsetX = GetScreenWidth() - (int)(environmentPalettes.size() * size * 0.5f);

            DrawRectangle(offsetX + (x * size), offsetY + (y * size), size, size, colorToDraw);
        }
    }
}
