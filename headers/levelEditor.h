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

    //refactor the editor so it is able to assign textures directly

    //i can switch from using TileType to just Tile as it has both the type and renderData

    int currentTileType = (int)TileType::VOID;
    
    int currentTexture = DEFAULT_INVALID_INDEX;

    Tile tempLevel[ROWS][COLS];

    Camera2D camera = {};

    IntPair mouseMatrixPosition = {0,0};

    std::vector<Texture2D>* activeTextureArray = nullptr;

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

/*

for having different sets of textures for solid tiles

You would essentially introduce a currentSolidPalette variable.

The Storage: You have a std::vector<std::vector<Texture2D>> solidPalettes.

    solidPalettes[0] = Grass Set (Corners, Edges, etc.)

    solidPalettes[1] = Stone Set

    solidPalettes[2] = Wood Set

The Selection: * Mouse Wheel + CTRL: Changes the currentSolidPalette index (switches between Grass/Stone/Wood).

    Mouse Wheel + SHIFT: Changes the currentTexture index (switches between Corner/Middle/Edge within that palette).

refactor GetActiveTextureArray adding a second parameter for the current palette



separate the types on the editor on these

platforms (platform stop included)

special physical tiles

misc (player spawn, goal)

enemies

*/