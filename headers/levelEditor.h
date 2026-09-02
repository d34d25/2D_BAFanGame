#pragma once

#include "leveldata.h"
#include "platform.h"
#include <vector>

struct IntPair
{
    int x = 0;
    int y = 0;
};

inline Vector2 ConvertFromIntPairToVector2(IntPair pair, int scale, int offsetX = 0, int offsetY = 0)
{
    return {(float)pair.x * scale + offsetX, (float)pair.y * scale + offsetY};
}

struct LevelEditor
{
    Tile tempLevel[LAYERS][COLS][ROWS];

    std::vector<Room> rooms = {};

    std::array<Color, MAX_PALETTE_COLS> currentPaletteColors = {};

    Camera2D camera = {};

    SpriteRenderData* activeRenderData = nullptr;

    std::string levelPath;

    std::string roomPath;

    Vector2 targetNeigbourPos = {0,0};

    IntPair mouseMatrixPosition = {0,0};

    Direction currentDirection = Direction::UP;

    SpriteFlipData currentData = {};

    int screenWidth, screenHeight;

    int chunkSize = 1;

    int maxChunkSize = 4;

    int currentLayer = 1;

    int currentTileType = (int)TileType::VOID;
    
    int currentTexture = 0;

    int currentVariant = 0;

    int currentPalette = 0;

    int currentAngle = 0;

    int currentBackgroundColor = 0;

    int currentBackgroundPalette = 0;

    int currentBackgroundIndex = -1;

    int currentRoomIndex = -1;

    bool roomMode = false;

    LevelEditor(int screenWidth, int screenHeight, std::string levelPath, std::string roomPath);

    ~LevelEditor() = default;

    void ExportLevel();

    void DrawRotatingSpikesRec(int currentType, Vector2 position, float size, SpriteFlipData data, Color color);

    void DrawRotatingSpikesSprite(int currentType, int frame, const SpriteRenderData& renderData, Vector2 position, float size, const SpriteFlipData& data);

    void Update();

    void Draw();

    inline bool HasReactiveAnimations(const TileType& type)
    {
        if(type > TileType::ENEMY_START && type < TileType::ENEMY_END) return true;

        return false;
    }

    inline void UpdateCamera()
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        {
            Vector2 delta = GetMouseDelta();

            camera.target.x -= delta.x / camera.zoom;
            camera.target.y -= delta.y / camera.zoom;
        }
    }

    inline bool IsNumKeyPressed()
    {
        return IsKeyPressed(KEY_ZERO) || 
        IsKeyPressed(KEY_ONE) || 
        IsKeyPressed(KEY_TWO) || 
        IsKeyPressed(KEY_THREE) || 
        IsKeyPressed(KEY_FOUR) || 
        IsKeyPressed(KEY_FIVE) || 
        IsKeyPressed(KEY_SIX) || 
        IsKeyPressed(KEY_SEVEN) || 
        IsKeyPressed(KEY_EIGHT) ||
        IsKeyPressed(KEY_NINE);
    }

    inline bool IsDirectionChangeKeyPressed()
    {
        return IsKeyPressed(KEY_E) || IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_V);
    }

    inline Vector2 GetMouseGridPosition(IntPair mousePosition)
    {
        return {
            mousePosition.x * TILE_SIZE + TILE_SIZE * 0.5f,
            mousePosition.y * TILE_SIZE + TILE_SIZE * 0.5f
        };
    }

    inline Vector2 GetMouseCell(IntPair mousePosition)
    {
        return {
            (float)mousePosition.x * TILE_SIZE,
            (float)mousePosition.y * TILE_SIZE
        };
    }

    inline void ResetRooms()
    {
        rooms.clear();

        float roomWidth = (float)(TILES_PER_ROOM_WIDHT * TILE_SIZE);
        float roomHeight = (float)(TILES_PER_ROOM_HEIGHT * TILE_SIZE);

        int worldScreenCols = COLS / TILES_PER_ROOM_WIDHT;
        int worldScreenRows = ROWS / TILES_PER_ROOM_HEIGHT;

        for(int i = 0; i < worldScreenCols; i++)
        {
            for(int j = 0; j < worldScreenRows; j++)
            {
                Room roomUnit;
                roomUnit.aabb = {
                    i * roomWidth,
                    j * roomHeight,
                    roomWidth,
                    roomHeight
                };

                rooms.push_back(roomUnit);
            }
        }
    }
};