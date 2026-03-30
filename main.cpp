#include <iostream>

#include "raylib.h"

#include "level.h"

int main()
{
    const int SCREEN_WIDTH = 800 * 2;
    const int SCREEN_HEIGHT = 450 * 2;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "");

    SetTargetFPS(60);

    float accumulator = 0.0f;
    float fixedDt = 1.0f / 60.0f;

    Level testLevel = Level();

    int iterations = 10;

    int gridSize = 50;

    testLevel.InitLevel(
        "assets/testLevel.png",
        gridSize,
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

    while (!WindowShouldClose())
    {
        //update

        float dt = GetFrameTime();
        accumulator += dt;

        while (accumulator >= fixedDt)
        {
            testLevel.UpdateLevel();
            accumulator -= fixedDt;
        }
        
        //-----

        BeginDrawing();

        ClearBackground(LIGHTGRAY);

        testLevel.DrawLevel();

        DrawFPS(10,10);

        EndDrawing();
    }

    CloseWindow();

    std::cout<<std::endl;
    
}