#include <iostream>

#include "raylib.h"

#include "levelmanager.h"

#include <chrono>

#include <memory>

int main()
{
    int screenScale = 6;

    int screenWidth = NATIVE_WIDTH * screenScale;
    int screenHeight = NATIVE_HEIGHT * screenScale;

    InitWindow(screenWidth, screenHeight, "");

    SetTargetFPS(60);

    float accumulator = 0.0f;
    
    float fixedDt = 1.0f / 60.0f;

    int iterations = 10;

    if(iterations < 1)
    {
        std::cout<<"ITERATIONS CAN'T BE LESS THAN 1"<<std::endl;

        CloseWindow();

        return 0;
    }

    InitManagerExtern();

    if(currentScreen == GameScreen::EDITOR)
    {
        screenWidth = 1600;

        SetWindowSize(screenWidth, screenHeight);
    }

    auto levelInitTimeStart = std::chrono::high_resolution_clock::now();

    ResetLevel(
        "levels/testLevel",
        "levels/testRooms",
        fixedDt,
        iterations
    );

    auto levelInitTimeEnd = std::chrono::high_resolution_clock::now();

    double levelInitTime = std::chrono::duration<double, std::milli>(levelInitTimeEnd - levelInitTimeStart).count();

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

        UpdateScreenInput();

        while (accumulator >= fixedDt)
        {
            auto allPhysicsTimerStart = std::chrono::high_resolution_clock::now();

            UpdateScreen(fixedDt, iterations);

            auto allPhysicsTimerEnd = std::chrono::high_resolution_clock::now();
            
            double physicsTime = std::chrono::duration<double, std::milli>(allPhysicsTimerEnd - allPhysicsTimerStart).count();

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

        DrawScreen();

        DrawFPS(10,10);

        auto drawingEnd = std::chrono::high_resolution_clock::now();
        
        double drawingTime = std::chrono::duration<double, std::milli>(drawingEnd - drawingStart).count();

        totalDrawingTime += drawingTime;
        drawingStepCount++;

        lastDrawingFrames[lastDrawingIndex] = drawingTime;

        lastDrawingIndex = (lastDrawingIndex + 1) % MAX_LAST_FRAMES;

        if(lastDrawingCount < MAX_LAST_FRAMES) lastDrawingCount++;

        EndDrawing();
    }

    std::cout<<"\n";

    std::cout<<"Level loading time: "<<levelInitTime<<" ms\n";

    std::cout<<"\n";

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

    std::cout<<"\n";
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

    std::cout<<"total platform count: "<<currLevel->GetPlatformCount()<<"\n";

    std::cout<<"\n";

    std::cout<<"total player update platform cache count: "<<currLevel->GetPlatformCache_Update()<<"\n";
    std::cout<<"total player physics platform cache count: "<<currLevel->GetPlatformCache_Physics()<<"\n";
    std::cout<<"total player render platform cache count: "<<currLevel->GetPlatformCache_Render()<<"\n";

    std::cout<<"\n";

    std::cout<<"total enemy count: "<<currLevel->GetEnemyCount()<<"\n";

    std::cout<<"total player enemy cache count: "<<currLevel->GetEnemyCache()<<"\n";
    std::cout<<"total player enemy physics cache count: "<<currLevel->GetEnemyCache_Physics()<<"\n";

    std::cout<<"Tile struct size: "<<(int)sizeof(Tile)<<"\n";

    std::cout<<"level map size: "<<(int)sizeof(currLevel->level)<<"\n";

    std::cout<<"level map size in tiles: "<<LAYERS * COLS * ROWS<<"\n";

    std::cout<<"Platform struct size: "<<(int)sizeof(Platform)<<"\n";

    std::cout<<"Enemy struct size: "<<(int)sizeof(Enemy)<<"\n";

    std::cout<<"\n";

    DestroyManager();

    CloseWindow();

    std::cout<<"width: "<<NATIVE_WIDTH<<"\n";
    std::cout<<"height: "<<NATIVE_HEIGHT<<"\n";

    std::cout<<"aspect ratio: "<<(float)((float)NATIVE_WIDTH / (float)NATIVE_HEIGHT)<<"\n";

    std::cout<<std::endl;   
}