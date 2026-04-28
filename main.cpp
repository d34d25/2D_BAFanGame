#include <iostream>

#include "raylib.h"

#include "level.h"

#include "levelEditor.h"

int main()
{
    const int SCREEN_WIDTH = 800 * 2;
    const int SCREEN_HEIGHT = 450 * 2;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "");

    SetTargetFPS(60);

    float accumulator = 0.0f;
    float fixedDt = 1.0f / 60.0f;

    Level testLevel = Level();

    LevelEditor editor = LevelEditor(SCREEN_WIDTH,SCREEN_HEIGHT, "levels/testLevel");

    int iterations = 10;

    testLevel.InitLevel(
        "levels/testLevel",
        fixedDt,
        iterations
    );

    if(iterations < 1)
    {
        std::cout<<"ITERATIONS CAN'T BE LESS THAN 1"<<std::endl;
        return 0;
    }

    testLevel.screenWidth = SCREEN_WIDTH;
    testLevel.screenHeight = SCREEN_HEIGHT;

    bool editorMode = false;

    while (!WindowShouldClose())
    {
        //update

        float dt = GetFrameTime();
        accumulator += dt;

        while (accumulator >= fixedDt)
        {
            //when making levels a list of level instances
            //wrap that logic in a level manager
            //hud and transitions and level switching are drawn and managed there
            
            if(!editorMode) testLevel.UpdateLevel();
            else editor.Update();

            accumulator -= fixedDt;
        }
        
        //-----

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        if(!editorMode) testLevel.DrawLevel();
        else editor.Draw();

        DrawFPS(10,10);

        EndDrawing();
    }

    CloseWindow();

    std::cout<<std::endl;
    
}