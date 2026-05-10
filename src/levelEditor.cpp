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
        std::cout<<"LEVEL EXPORT FAILED"<<std::endl;
    }
}

LevelEditor::LevelEditor(int screenWidth, int screenHeight, const char* levelPath)
{
    this->levelPath = levelPath;

    camera.zoom = 1.0f;

    camera.offset = {screenWidth * 0.5f, screenHeight * 0.5f};

    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    int worldWidth = COLS * gridSize;
    int worldHeight = ROWS * gridSize;

    int halfWorldWidth = (int)floor(worldWidth * 0.5f);
    int halfWorldHeight = (int)floor(worldHeight * 0.5f);

    camera.target = {(float)halfWorldWidth, (float)halfWorldHeight};

    LoadAssets();

    currentTexture = 0;
}

LevelEditor::~LevelEditor()
{
    UnloadAssets();
}

void LevelEditor::Update()
{
    UpdateCamera();

    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

    int i = (int)floor(mouseWorldPos.x / gridSize);
    int j = (int)floor(mouseWorldPos.y / gridSize);

    i = Clamp(i, 0, ROWS - 1);
    j = Clamp(j, 0, COLS - 1);

    mouseMatrixPosition = {i,j};

    activeRenderData = GetTileActiveRenderData((TileType)currentTileType, currentVariant);

    std::vector<SpriteRenderData>* activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

    if(!activeRenderDataList) currentVariant = 0;

    if(IsKeyPressed(KEY_TAB))
    {
        currentLayer++;

        if(currentLayer >= LAYERS) currentLayer = 0;
    }

    if(IsNumKeyPressed())
    {
        if(IsKeyPressed(KEY_ZERO)) currentTileType = (int)TileType::VOID;

        if(IsKeyPressed(KEY_ONE)) currentTileType = (int)TileType::TILE_START + 1;

        if(IsKeyPressed(KEY_TWO)) currentTileType = (int)TileType::SPIKE_START + 1;

        if(IsKeyPressed(KEY_THREE)) currentTileType = (int)TileType::PLATFORM_START + 1;

        if(IsKeyPressed(KEY_FOUR)) currentTileType = (int)TileType::MISC_START + 1;

        if(IsKeyPressed(KEY_FIVE)) currentTileType = (int)TileType::DECO;

        activeRenderData = GetTileActiveRenderData((TileType)currentTileType, 0);

        activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

        currentVariant = 0;

        currentTexture = 0;

        currentAngle = 0;

        currentData = {false, false};

        currentDirection = Direction::UP;
    }

    float mouseWheel = GetMouseWheelMove();

    if(IsKeyDown(KEY_LEFT_ALT) && mouseWheel != 0)
    {
        float cameraZoomFactor = 0.25f;

        if(mouseWheel > 0) camera.zoom += cameraZoomFactor;
        else if(mouseWheel < 0) camera.zoom -= cameraZoomFactor;

        camera.zoom = Clamp(camera.zoom, 0.1f,10.0f);
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

    //variant cycling
    if(mouseWheel != 0 && IsKeyDown(KEY_SPACE))
    {
        std::vector<SpriteRenderData>* activeRenderDataList = GetTileActiveRenderDataList((TileType)currentTileType);

        if(activeRenderDataList)
        {
            int direction = (mouseWheel > 0) ? 1 : -1;

            currentVariant += direction;

            if(currentVariant < 0) currentVariant = 0;
            else if(currentVariant >= activeRenderDataList->size()) currentVariant = activeRenderDataList->size() - 1;
        }
    }

    //texture cycling
    if(mouseWheel != 0 && IsKeyDown(KEY_LEFT_SHIFT))
    {
        if(activeRenderData && !activeRenderData->animationFrames.empty())
        {
            int direction = (mouseWheel > 0) ? 1 : -1;

            currentTexture += activeRenderData->spacing * direction;

            int frameCount = (int)activeRenderData->animationFrames.size();

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
        else
        {
            direction = 0;
        }

        currentTileType += direction;

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

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if(currentTileType != (int)TileType::VOID
            && currentTileType != (int)targetTile.type
            && !IsTypeInvalid((TileType)currentTileType)
        )
        {
            if((TileType)currentTileType == TileType::PLAYER_SPAWN)
            {
                for(int l = 0; l < LAYERS; l++)
                {
                    for(int i = 0; i < ROWS; i++)
                    {
                        for(int j = 0; j < COLS; j++)
                        {
                            if(tempLevel[l][i][j].type == TileType::PLAYER_SPAWN)
                            {
                                tempLevel[l][i][j] = {};
                            }
                        }
                    }
                }   
            }

            targetTile.type = (TileType)currentTileType;
            targetTile.textureIndex = currentTexture;
            targetTile.variantIndex = currentVariant;

            targetTile.gameObj.transform.position = GetMouseGridPosition(mouseMatrixPosition);

            targetTile.gameObj.transform.scale = tileScale;

            targetTile.gameObj.transform.angle = currentAngle;

            targetTile.gameObj.data = currentData;

            targetTile.gameObj.direction = currentDirection;
        }

    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if(currentTileType != (int)TileType::VOID)
        {
            targetTile = {};
        }
    }

    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_C))
    {
        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLS; j++)
            {
                tempLevel[currentLayer][i][j] = {};
            }
        }
    }

    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_C) && IsKeyDown(KEY_LEFT_CONTROL))
    {
        for(int l = 0; l < LAYERS; i++)
        {
            for(int i = 0; i < ROWS; i++)
            {
                for(int j = 0; j < COLS; j++)
                {
                    tempLevel[l][i][j] = {};
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
        LoadLevelData(levelPath, tempLevel);
    }
}

void LevelEditor::Draw()
{
    int worldWidth = COLS * gridSize;
    int worldHeight = ROWS * gridSize;

    int halfWorldWidth = (int)floor(worldWidth * 0.5f);
    int halfWorldHeight = (int)floor(worldHeight * 0.5f);

    BeginMode2D(camera);

    TileRange cameraTileRange = CalculateTileRange(
        camera.target.x, camera.target.y, 100
    );
    
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

                Vector2 tileSize = {gridSize, gridSize};

                float offsetX = 0;
                float offsetY = 0;

                if(type == TileType::HORIZONALT_MOVING_PLATFORM || type == TileType::VERTICAL_MOVING_PLATFORM)
                {
                    tileSize.x = gridSize * 3.0f;
                    tileSize.y = gridSize * 0.3f;
                    offsetX = -gridSize;
                    offsetY = tileSize.y;
                }

                if(tile.textureIndex < 0 || !tileRenderData)
                {
                    if(IsColorOf(color, BLANK)) continue;

                    DrawRectangle(i * gridSize + offsetX, j * gridSize + offsetY, tileSize.x, tileSize.y, {color.a,color.g, color.b, alpha});
                }
                else 
                {
                    DrawSprite(tile.gameObj.transform, tileRenderData, tile.gameObj.data, tile.textureIndex, layerTint);
                }
            }
        }
    }

    //preview
    Color color = GetTileColor((TileType)currentTileType);

    Color previewColor = color;

    if(currentTileType != (int)TileType::VOID) previewColor.a = 50;

    Vector2 tileSize = {gridSize, gridSize};
    
    Transform2D previewTransform;

    previewTransform.position = GetMouseGridPosition(mouseMatrixPosition);
    previewTransform.scale = tileScale;
    previewTransform.angle = currentAngle;

    float offsetX = 0;
    float offsetY = 0;

    if(currentTileType == (int)TileType::HORIZONALT_MOVING_PLATFORM || currentTileType == (int)TileType::VERTICAL_MOVING_PLATFORM)
    {
        tileSize.x = gridSize * 3.0f;
        tileSize.y = gridSize * 0.3f;
        offsetX = -gridSize;
        offsetY =  tileSize.y;
    }

    if(currentTexture < 0 || !activeRenderData)
    {
        DrawRectangle(
            previewTransform.position.x - (gridSize * 0.5f) + offsetX, 
            previewTransform.position.y - (gridSize * 0.5f) + offsetY,
            tileSize.x, tileSize.y, 
            previewColor
        );
    }
    else
    {
        previewColor = WHITE;
        previewColor.a = 100;

        DrawSprite(previewTransform, activeRenderData, currentData, currentTexture, previewColor);
    }

    //grid
    for(int i = 0; i <= worldWidth; i+= gridSize)
    {
        if(i == halfWorldWidth) continue;

        DrawLine(i, 0, i, worldWidth, GRAY);
    }

    for(int i = 0; i <= worldHeight; i+= gridSize)
    {
        if(i == halfWorldHeight) continue;

        DrawLine(0, i, worldHeight, i, GRAY);
    }

    DrawLine(halfWorldWidth, -worldWidth, halfWorldWidth, worldWidth, GREEN);

    DrawLine(-worldHeight, halfWorldHeight, worldHeight, halfWorldHeight, GREEN);

    DrawCircle(halfWorldWidth, halfWorldHeight, 5,GREEN);

    DrawRectangleLines(
        mouseMatrixPosition.x * gridSize, 
        mouseMatrixPosition.y * gridSize,
        gridSize, gridSize, RED
    );


    EndMode2D();
    
    //menu

    int ypos = 50;
    int spacing = 30;

    DrawText(TextFormat("mouse x: %i", mouseMatrixPosition.x), 10, ypos, 20, BLACK);

    DrawText(TextFormat("mouse y: %i", mouseMatrixPosition.y), 10, ypos + spacing, 20, BLACK);

    DrawText(GetTileTypeText((TileType)currentTileType), 10, ypos + spacing * 2, 20, BLACK);
    DrawText(TextFormat("tileType: %i", currentTileType ), 10, ypos + spacing * 3,20,BLACK);

    DrawText(TextFormat("current layer: %i", currentLayer), 10, ypos + spacing * 4, 20, RED);

    DrawText(TextFormat("current angle: %i", currentAngle), 10, ypos + spacing * 5, 20, RED);

    DrawText(TextFormat("current flip x: %i", currentData.flipX), 10, ypos + spacing * 6, 20, RED);
    DrawText(TextFormat("current flip y: %i", currentData.flipY), 10, ypos + spacing * 7, 20, RED);

    DrawText(GetDirectionText(currentDirection), 10, ypos + spacing * 8, 20, RED);
}
