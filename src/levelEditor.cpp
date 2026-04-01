#include "levelEditor.h"

#include<iostream>

void LevelEditor::ExportLevel()
{
    int dataSize = ROWS * COLS * sizeof(TileType);

    if(SaveFileData("testLevel", tempLevel, dataSize))
    {
        std::cout<<"LEVEL EXPORT SUCCSESSFULLY"<<"\n";
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

    TileType* loadedTypes = (TileType*)fileData;

    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            TileType type = loadedTypes[i * COLS + j];

            tempLevel[i][j] = type;
        }
    }

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

    if(IsKeyPressed(KEY_ZERO)) currentTileType = TileType::VOID;

    if(IsKeyPressed(KEY_ONE)) currentTileType = TileType::SOLID;

    if(IsKeyPressed(KEY_TWO)) currentTileType = TileType::GOAL;

    float mouseWheel = GetMouseWheelMove();

    if(IsKeyDown(KEY_LEFT_ALT) && mouseWheel != 0)
    {
        float cameraZoomFactor = 0.25f;

        if(mouseWheel > 0) camera.zoom += cameraZoomFactor;
        else if(mouseWheel < 0) camera.zoom -= cameraZoomFactor;

        camera.zoom = Clamp(camera.zoom, 0.1f,10.0f);
    }

    if(mouseWheel != 0 && currentTileType > TileType::SOLID && !IsKeyDown(KEY_LEFT_ALT))
    {
        static int specialTileIndex = (int)TileType::GOAL;

        if(mouseWheel > 0) specialTileIndex++;
        else if(mouseWheel < 0) specialTileIndex--;

        if(specialTileIndex < 2) specialTileIndex = (int)TileType::COUNT - 1;
        if(specialTileIndex >= (int)TileType::COUNT) specialTileIndex = (int)TileType::GOAL;

        currentTileType = (TileType)specialTileIndex;
    }

    TileType& currentTile = tempLevel[mouseMatrixPosition.x][mouseMatrixPosition.y];

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        if(currentTileType > TileType::VOID
            && currentTileType != currentTile
            && currentTileType < TileType::COUNT
        )
        {
            currentTile = currentTileType;
        }

    }
    else if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        if(currentTileType != TileType::VOID)
        {
            currentTile = TileType::VOID;
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
            TileType type = tempLevel[i][j];

            if(type == TileType::VOID || type >= TileType::COUNT) continue;

            Color color = GetTileColor(type);

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

            DrawRectangle(i * gridSize + offset, j * gridSize, tileWidth, tileHeight, color);
        }
    }
    

    Color color = GetTileColor((TileType)currentTileType);

    Color previewColor = color;

    if(currentTileType != TileType::VOID) previewColor.a = 50;

    float tileWidth = gridSize;
    float tileHeight = gridSize;
    float offset = 0;

    if(currentTileType == TileType::HORIZONALT_MOVING_PLATFORM || currentTileType == TileType::VERTICAL_MOVING_PLATFORM)
    {
        tileWidth = gridSize * 3.0f;
        tileHeight = gridSize * 0.3f;
        offset = -gridSize;
    }

    DrawRectangle(
        mouseMatrixPosition.x * gridSize + offset, 
        mouseMatrixPosition.y * gridSize,
        tileWidth, tileHeight, 
        previewColor
    );

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

    DrawText(TextFormat("mouse x: %i", mouseMatrixPosition.x), 10, 10, 20, BLACK);

    DrawText(TextFormat("mouse y: %i", mouseMatrixPosition.y), 10, 30, 20, BLACK);

    DrawText(GetTileTypeText(currentTileType), 10, 50, 20, BLACK);
}
