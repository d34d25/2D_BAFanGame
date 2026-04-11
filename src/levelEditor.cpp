#include "levelEditor.h"

#include<iostream>
#include<cstring>

void LevelEditor::ExportLevel()
{
    int dataSize = ROWS * COLS * sizeof(Tile);

    if(SaveFileData("testLevel", tempLevel, dataSize))
    {
        std::cout<<"LEVEL EXPORT SUCCEED"<<"\n";
    }
    else 
    {
        std::cout<<"LEVEL EXPORT FAILED"<<std::endl;
    }
}

void LevelEditor::ImportLevel(const char* levelPath)
{
    int dataSize = 0;

    unsigned char* fileData = LoadFileData(levelPath, &dataSize);

    if(fileData == nullptr) return;

    if(dataSize != (ROWS * COLS * sizeof(Tile))) return;

    memcpy(tempLevel, fileData, dataSize);
    
    UnloadFileData(fileData);
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

    if(!activeRenderData || activeRenderData->animationFrames.empty()) currentTexture = DEFAULT_INVALID_INDEX;
    else currentTexture = 0;
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

    activeRenderData = GetActiveRenderData((TileType)currentTileType);

    if(IsNumKeyPressed())
    {
        if(IsKeyPressed(KEY_ZERO)) currentTileType = (int)TileType::VOID;

        if(IsKeyPressed(KEY_ONE)) currentTileType = (int)TileType::TILE_START + 1;

        if(IsKeyPressed(KEY_TWO)) currentTileType = (int)TileType::PLATFORM_START + 1;

        if(IsKeyPressed(KEY_THREE)) currentTileType = (int)TileType::MISC_START + 1;

        activeRenderData = GetActiveRenderData((TileType)currentTileType);

        if(!activeRenderData || activeRenderData->animationFrames.empty()) currentTexture = DEFAULT_INVALID_INDEX;
        else currentTexture = 0;
    }

    float mouseWheel = GetMouseWheelMove();

    if(IsKeyDown(KEY_LEFT_ALT) && mouseWheel != 0)
    {
        float cameraZoomFactor = 0.25f;

        if(mouseWheel > 0) camera.zoom += cameraZoomFactor;
        else if(mouseWheel < 0) camera.zoom -= cameraZoomFactor;

        camera.zoom = Clamp(camera.zoom, 0.1f,10.0f);
    }

    //texture cycling
    if(mouseWheel != 0 && IsKeyDown(KEY_LEFT_SHIFT))
    {
        if(activeRenderData && !activeRenderData->animationFrames.empty())
        {
            int direction = (mouseWheel > 0) ? 1 : -1;

            currentTexture += activeRenderData->spacing * direction;

            if(currentTexture < activeRenderData->startFrame) currentTexture = activeRenderData->endFrame - activeRenderData->spacing;
            else if (currentTexture >= activeRenderData->endFrame) currentTexture = activeRenderData->startFrame;
        }
        else
        {
            currentTexture = DEFAULT_INVALID_INDEX;
        }
    }

    //tile type cycling
    if(mouseWheel != 0 && !IsKeyDown(KEY_LEFT_ALT) && !IsKeyDown(KEY_LEFT_SHIFT))
    {
        int direction = (mouseWheel > 0) ? 1 : -1;

        int start = (int)TileType::VOID;

        int end = (int)TileType::COUNT;

        if(currentTileType >= (int)TileType::TILE_START && currentTileType <= (int)TileType::TILE_END)
        {
            start = (int)TileType::TILE_START;
            end = (int)TileType::TILE_END;
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

        activeRenderData = GetActiveRenderData((TileType)currentTileType);

        if(!activeRenderData || activeRenderData->animationFrames.empty()) currentTexture = DEFAULT_INVALID_INDEX;
        else currentTexture = activeRenderData->startFrame;
    }

    TileType& currentTile = tempLevel[mouseMatrixPosition.x][mouseMatrixPosition.y].type;
    int& currentTileTextureIndex = tempLevel[mouseMatrixPosition.x][mouseMatrixPosition.y].textureIndex;

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if(currentTileType != (int)TileType::VOID
            && currentTileType != (int)currentTile
            && !IsTypeInvalid((TileType)currentTileType)
        )
        {
            if((TileType)currentTileType == TileType::PLAYER_SPAWN)
            {
                for(int i = 0; i < ROWS; i++)
                {
                    for(int j = 0; j < COLS; j++)
                    {
                        if(tempLevel[i][j].type == TileType::PLAYER_SPAWN)
                        {
                            tempLevel[i][j].type = TileType::VOID;
                            tempLevel[i][j].textureIndex = DEFAULT_INVALID_INDEX;
                        }
                    }
                }
            }

            currentTile = (TileType)currentTileType;
            currentTileTextureIndex = currentTexture;
        }

    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if(currentTileType != (int)TileType::VOID)
        {
            currentTile = TileType::VOID;
            currentTileTextureIndex = DEFAULT_INVALID_INDEX;
        }
    }

    if(IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_C))
    {
        for(int i = 0; i < ROWS; i++)
        {
            for(int j = 0; j < COLS; j++)
            {
                tempLevel[i][j] = {};
            }
        }
    }

    if(IsKeyPressed(KEY_F5))
    {
        ExportLevel();
    }
    else if(IsKeyPressed(KEY_F9))
    {
        ImportLevel(levelPath);
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
    
    for(int i = cameraTileRange.startX; i <= cameraTileRange.endX; i++)
    {
        for(int j = cameraTileRange.startY; j <= cameraTileRange.endY; j++)
        {
            Tile tile = tempLevel[i][j];

            TileType type = tile.type;

            int tileTextureId = tile.textureIndex;

            if(type == TileType::VOID || type >= TileType::COUNT) continue;

            Color color = GetTileColor(type);

            SpriteRenderData* tileRenderData = GetActiveRenderData(type);

            if(IsColorOf(color, BLANK)) continue;

            float tileWidth = gridSize;
            float tileHeight = gridSize;

            float offset = 0;

            if(type == TileType::HORIZONALT_MOVING_PLATFORM || type == TileType::VERTICAL_MOVING_PLATFORM)
            {
                tileWidth = gridSize * 3.0f;
                tileHeight = gridSize * 0.3f;
                offset = -gridSize;
            }

            if(tileTextureId < 0 || !tileRenderData)
            {
                DrawRectangle(i * gridSize + offset, j * gridSize, tileWidth, tileHeight, color);
            }
            else 
            {

                DrawTile(tileRenderData, tileTextureId, {(float) i * gridSize, (float) j * gridSize}, gridSize);
            }
        }
    }
    

    Color color = GetTileColor((TileType)currentTileType);

    Color previewColor = color;

    if(currentTileType != (int)TileType::VOID) previewColor.a = 50;

    float tileWidth = gridSize;
    float tileHeight = gridSize;
    float offset = 0;

    if(currentTileType == (int)TileType::HORIZONALT_MOVING_PLATFORM || currentTileType == (int)TileType::VERTICAL_MOVING_PLATFORM)
    {
        tileWidth = gridSize * 3.0f;
        tileHeight = gridSize * 0.3f;
        offset = -gridSize;
    }

    if(currentTexture < 0 || !activeRenderData)
    {
        DrawRectangle(
            mouseMatrixPosition.x * gridSize + offset, 
            mouseMatrixPosition.y * gridSize,
            tileWidth, tileHeight, 
            previewColor
        );
    }
    else
    {
        previewColor = WHITE;
        previewColor.a = 100;

        DrawTile(activeRenderData, currentTexture, ConvertFromIntPairToVector2(mouseMatrixPosition, gridSize), gridSize, previewColor);
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
}
