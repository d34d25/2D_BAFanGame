#pragma once

#include "leveldata.h"
#include "platform.h"
#include <vector>

struct IntPair
{
    int x = 0;
    int y = 0;
};

class LevelEditor
{
private:

    int screenWidth, screenHeight;

    TileType currentTileType = TileType::VOID;

    TileType tempLevel[ROWS][COLS];

    Camera2D camera = {};

    IntPair mouseMatrixPosition = {0,0};

    inline void UpdateCamera()
    {
        if(IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
        {
            Vector2 delta = GetMouseDelta();

            camera.target.x -= delta.x / camera.zoom;
            camera.target.y -= delta.y / camera.zoom;
        }
    }

    void ExportLevel();

    void ImportLevel(const char* levelPath);
    
    const char* levelPath;

public:

    LevelEditor(int screenWidth, int screenHeight, const char* levelPath);

    ~LevelEditor() = default;

    void Update();

    void Draw();

};