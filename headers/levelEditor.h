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

class LevelEditor
{
private:

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
    
    std::array<Color, MAX_PALETTE_COLS> currentPaletteColors = {};

    bool roomMode = false;

    std::vector<Room> rooms = {};

    Vector2 targetNeigbourPos = {0,0};

    Direction currentDirection = Direction::UP;

    EntityData currentData = {};

    Tile tempLevel[LAYERS][COLS][ROWS];

    Camera2D camera = {};

    IntPair mouseMatrixPosition = {0,0};

    SpriteRenderData* activeRenderData = nullptr;

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
            mousePosition.x * GRID_SIZE + GRID_SIZE * 0.5f,
            mousePosition.y * GRID_SIZE + GRID_SIZE * 0.5f
        };
    }

    inline Vector2 GetMouseCell(IntPair mousePosition)
    {
        return {
            (float)mousePosition.x * GRID_SIZE,
            (float)mousePosition.y * GRID_SIZE
        };
    }

    inline void ResetRooms()
    {
        rooms.clear();

        float roomWidth = (float)(TILES_PER_ROOM_WIDHT * GRID_SIZE);
        float roomHeight = (float)(TILES_PER_ROOM_HEIGHT * GRID_SIZE);

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

    void ExportLevel();
    
    const char* levelPath;

    const char* roomPath;

    void DrawRotatingSpikesRec(int currentType, Vector2 position, float size, EntityData data, Color color);

    void DrawRotatingSpikesSprite(int currentType, int frame, const SpriteRenderData& renderData, Vector2 position, float size, const EntityData& data);

public:

    LevelEditor(int screenWidth, int screenHeight, const char* levelPath, const char* roomPath);

    ~LevelEditor();

    void Update();

    void Draw();

};