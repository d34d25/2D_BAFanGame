#include <iostream>

#include "raylib.h"

#include "level.h"

#include "levelEditor.h"

#include <chrono>

#include <memory>

int main()
{
    bool editorMode = true;

    int screenWidth = 900;
    int screenHeight = 900;

    if(editorMode) screenWidth = 1600;

    int totalCanvasWidth = CANVAS_WIDTH;
    int totalCanvasHeight = GAMEPLAY_CANVAS_HEIGHT + UI_CANVAS_HEIGHT;

    InitWindow(screenWidth, screenHeight, "");

    SetTargetFPS(60);

    LoadAssets();

    float accumulator = 0.0f;
    float fixedDt = 1.0f / 60.0f;

    RenderTexture2D gameplayCanvas = LoadRenderTexture(CANVAS_WIDTH, GAMEPLAY_CANVAS_HEIGHT);

    RenderTexture2D uiCanvas = LoadRenderTexture(CANVAS_WIDTH, UI_CANVAS_HEIGHT);

    SetTextureFilter(gameplayCanvas.texture, TEXTURE_FILTER_POINT);

    SetTextureFilter(uiCanvas.texture, TEXTURE_FILTER_POINT);

    std::unique_ptr testLevel = std::make_unique<Level>();

    std::unique_ptr editor = std::make_unique<LevelEditor>(CANVAS_WIDTH,GAMEPLAY_CANVAS_HEIGHT, "levels/testLevel", "levels/testRooms");

    int iterations = 10;

    testLevel->InitLevel(
        "levels/testLevel",
        "levels/testRooms",
        fixedDt,
        iterations
    );

    if(iterations < 1)
    {
        std::cout<<"ITERATIONS CAN'T BE LESS THAN 1"<<std::endl;
        return 0;
    }

    testLevel->totalCanvasWidth = totalCanvasWidth;
    testLevel->totalCanvasHeight = totalCanvasHeight;

    testLevel->gameplayCanvas = gameplayCanvas;
    testLevel->uiCanvas = uiCanvas;

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

        if(!editorMode) testLevel->UpdatePlayerInput();

        while (accumulator >= fixedDt)
        {
            //when making levels a list of level instances
            //wrap that logic in a level manager
            //hud and transitions and level switching are drawn and managed there
            
            auto allPhysicsTimerStart = std::chrono::high_resolution_clock::now();

            if(!editorMode) testLevel->UpdateLevel();
            else editor->Update();

            auto allPhysicsTimerEnd = std::chrono::high_resolution_clock::now();
            
            double physicsTime = std::chrono::duration<double, std::milli>(allPhysicsTimerEnd - allPhysicsTimerStart).count();

            //if(!editorMode) std::cout<<"Total UpdateLevel time: "<<physicsTime<<" ms \n";

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

        if(!editorMode) testLevel->DrawLevel();
        else
        {
            editor->Draw();
        }

        DrawFPS(10,10);

        auto drawingEnd = std::chrono::high_resolution_clock::now();
        
        double drawingTime = std::chrono::duration<double, std::milli>(drawingEnd - drawingStart).count();
        //if(!editorMode) std::cout<<"Drawing total time: "<<drawingTime<<" ms \n";

        totalDrawingTime += drawingTime;
        drawingStepCount++;

        lastDrawingFrames[lastDrawingIndex] = drawingTime;

        lastDrawingIndex = (lastDrawingIndex + 1) % MAX_LAST_FRAMES;

        if(lastDrawingCount < MAX_LAST_FRAMES) lastDrawingCount++;

        EndDrawing();
    }

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

    std::cout<<"total platform count: "<<testLevel->GetPlatformCount()<<"\n";

    std::cout<<"\n";

    std::cout<<"total player update platform cache count: "<<testLevel->GetPlatformCache_Update()<<"\n";
    std::cout<<"total player physics platform cache count: "<<testLevel->GetPlatformCache_Physics()<<"\n";
    std::cout<<"total player render platform cache count: "<<testLevel->GetPlatformCache_Render()<<"\n";

    std::cout<<"\n";

    std::cout<<"total enemy count: "<<testLevel->GetEnemyCount()<<"\n";

    std::cout<<"total player enemy cache count: "<<testLevel->GetEnemyCache()<<"\n";
    std::cout<<"total player enemy physics cache count: "<<testLevel->GetEnemyCache_Physics()<<"\n";

    std::cout<<"Tile struct size: "<<(int)sizeof(Tile)<<"\n";

    std::cout<<"level map size: "<<(int)sizeof(testLevel->level)<<"\n";

    std::cout<<"Platform struct size: "<<(int)sizeof(Platform)<<"\n";

    std::cout<<"Enemy struct size: "<<(int)sizeof(Enemy)<<"\n";

    std::cout<<"\n";

    testLevel.reset();
    
    editor.reset();

    UnloadAssets();
    testLevel.reset();
    
    editor.reset();
    UnloadRenderTexture(gameplayCanvas);

    UnloadRenderTexture(uiCanvas);

    CloseWindow();

    std::cout<<"width: "<<totalCanvasWidth<<"\n";
    std::cout<<"height: "<<totalCanvasHeight<<"\n";

    std::cout<<"aspect ratio: "<<(float)((float)totalCanvasWidth / (float)totalCanvasHeight)<<"\n";

    std::cout<<std::endl;   
}