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

    const int MAX_LAST_FRAMES = 120;

    double lastPhysicsFrames[MAX_LAST_FRAMES] = {0.0};
    int lastPhysicsCount = 0;
    int lastPhysicsIndex = 0;

    int physicsStepCount = 0;
    double totalPhysicsTime = 0.0;

    double lastDrawingFrames[MAX_LAST_FRAMES] = {0.0};
    int lastDrawingCount = 0;
    int lastDrawingIndex = 0;

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

            if(!editorMode) std::cout<<"Total UpdateLevel time: "<<physicsTime<<" ms \n";

            totalPhysicsTime += physicsTime;
            physicsStepCount++;

            lastPhysicsFrames[lastPhysicsIndex] = physicsTime;

            lastPhysicsIndex = (lastPhysicsIndex + 1) % MAX_LAST_FRAMES;

            if(lastPhysicsCount < MAX_LAST_FRAMES) lastPhysicsCount++;

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
        if(!editorMode) std::cout<<"Drawing total time: "<<drawingTime<<" ms \n";

        totalDrawingTime += drawingTime;
        drawingStepCount++;

        lastDrawingFrames[lastDrawingIndex] = drawingTime;

        lastDrawingIndex = (lastDrawingIndex + 1) % MAX_LAST_FRAMES;

        if(lastDrawingCount < MAX_LAST_FRAMES) lastDrawingCount++;

        EndDrawing();
    }

    std::cout<<"========================AVERAGE MS (ALL FRAMES)===========================\n";

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

    std::cout<<"=====================AVERAGE MS (LAST 120 FRAMES)===========================\n";

    if(lastPhysicsCount > 0)
    {
        double sum = 0.0f;

        for(int i = 0; i < lastPhysicsCount; i++) sum += lastPhysicsFrames[i];

        double avgPhysicsLastTime = sum / lastPhysicsCount;

        std::cout<<"Average last physics time: "<<avgPhysicsLastTime<<" ms (over "<<MAX_LAST_FRAMES<<" frames)\n";
    }

    if(lastDrawingCount > 0)
    {
        double sum = 0.0f;

        for(int i = 0; i < lastDrawingCount; i++) sum += lastDrawingFrames[i];

        double avgDrawingLastTime = sum / lastDrawingCount;

        std::cout<<"Average last drawing time: "<<avgDrawingLastTime<<" ms (over "<<MAX_LAST_FRAMES<<" frames)\n";
    }

    std::cout<<"total platform count: "<<testLevel.GetPlatformCount()<<"\n";

    std::cout<<"\n";

    std::cout<<"total player update platform cache count: "<<testLevel.GetPlayerPlatformCache_Update()<<"\n";
    std::cout<<"total player physics platform cache count: "<<testLevel.GetPlayerPlatformCache_Physics()<<"\n";
    std::cout<<"total player render platform cache count: "<<testLevel.GetPlayerPlatformCache_Render()<<"\n";

    std::cout<<"\n";

    std::cout<<"total enemy count: "<<testLevel.GetEnemyCount()<<"\n";

    std::cout<<"total player enemy cache count: "<<testLevel.GetPlayerEnemyCache()<<"\n";
    std::cout<<"total player enemy physics cache count: "<<testLevel.GetPlayerEnemyCache_Physics()<<"\n";

    std::cout<<"\n";

    CloseWindow();

    std::cout<<std::endl;
    
}