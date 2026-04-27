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

    int currentTileType = (int)TileType::VOID;
    
    int currentTexture = DEFAULT_INVALID_INDEX;

    int currentVariant = 0;

    Tile tempLevel[ROWS][COLS];

    Camera2D camera = {};

    IntPair mouseMatrixPosition = {0,0};

    SpriteRenderData* activeRenderData = nullptr;

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

    void ExportLevel();

    void ImportLevel(const char* levelPath);
    
    const char* levelPath;

public:

    LevelEditor(int screenWidth, int screenHeight, const char* levelPath);

    ~LevelEditor();

    void Update();

    void Draw();

};