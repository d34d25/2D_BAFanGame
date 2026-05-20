#include <iostream>

#include "raylib.h"

#include "level.h"

#include "levelEditor.h"

#include <chrono>

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

    int physicsStepCount = 0;
    double totalPhysicsTime = 0.0;

    double totalDrawingTime = 0.0;
    int drawingStepCount = 0;

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
            
            auto allPhysicsTimerStart = std::chrono::high_resolution_clock::now();

            if(!editorMode) testLevel.UpdateLevel();
            else editor.Update();

            auto allPhysicsTimerEnd = std::chrono::high_resolution_clock::now();
            
            double physicsTime = std::chrono::duration<double, std::milli>(allPhysicsTimerEnd - allPhysicsTimerStart).count();

            std::cout<<"Total UpdateLevel time: "<<physicsTime<<" ms \n";

            totalPhysicsTime += physicsTime;
            physicsStepCount++;

            accumulator -= fixedDt;
        }
        
        //-----

        BeginDrawing();

        auto drawingStart = std::chrono::high_resolution_clock::now();

        ClearBackground(LIGHTGRAY);

        if(!editorMode) testLevel.DrawLevel();
        else editor.Draw();

        DrawFPS(10,10);

        auto drawingEnd = std::chrono::high_resolution_clock::now();
        
        double drawingTime = std::chrono::duration<double, std::milli>(drawingEnd - drawingStart).count();
        std::cout<<"Drawing total time: "<<drawingTime<<" ms \n";

        totalDrawingTime += drawingTime;
        drawingStepCount++;

        EndDrawing();
    }

    std::cout<<"====================================================\n";

    if(physicsStepCount > 0)
    {
        double avgPhysicsTime = totalPhysicsTime / physicsStepCount;

        std::cout<<"Average physics time: "<<avgPhysicsTime<<" ms (over "<<physicsStepCount<<" frames)\n";
    }

    if(drawingStepCount > 0)
    {
        double avgDrawingTime = totalDrawingTime / drawingStepCount;

        std::cout<<"Average drawing time: "<<avgDrawingTime<<" ms (over "<<drawingStepCount<<" frames)\n";
    }

    CloseWindow();

    std::cout<<std::endl;
    
}