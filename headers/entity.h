#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

const float MASS = 1;

const Vector2 MAX_SPEED = {500,500};

struct SimpleBody2D
{
private:

    Vector2 finalVelocity = {0,0};

    Vector2 acceleration = {0,0};

public:

    Vector2 velocity = {0,0};
    Vector2 altVelocity = {0,0};

    Vector2 force = {0,0};

    float damping = 4.0f;

    bool hasGravity = false;

    inline void AddVelocities()
    {
        finalVelocity = Vector2Add(velocity, altVelocity);
    }

    void UpdateVelocity(float dt, int iterations, float gravity);

    inline void UpdatePositionX(float dt, int iterations, float* xpos)
    {
        float subDt = dt / iterations;

        *xpos += finalVelocity.x * subDt;
    }

    inline void UpdatePositionY(float dt, int iterations, float* ypos)
    {
        float subDt = dt / iterations;

        *ypos += finalVelocity.y * subDt;
    }

    inline Vector2 GetFinalVelocity()
    {
        return finalVelocity;
    }
};

struct GameObject
{

public:

    Vector2 position = {0,0};

    float scale  = 0;
    float rotation = 0;

    Rectangle aabb = {0,0,50,50};

    SimpleBody2D body = {};

    bool canEntityCollidePhysically = false;
    bool canPlatformCollidePhysically = false;

    inline void UpdateAABB()
    {
        aabb.x = position.x - aabb.width * 0.5f;
        aabb.y = position.y - aabb.height * 0.5f;
    }

    inline void UpdateVelocity(float dt, int iterations, float gravity)
    {
        body.UpdateVelocity(dt,iterations, gravity);
    }

    inline void UpdatePositionX(float dt, int iterations)
    {
        body.UpdatePositionX(dt, iterations, &position.x);
        UpdateAABB();
    }

    inline void UpdatePositionY(float dt, int iterations)
    {
        body.UpdatePositionY(dt, iterations, &position.y);
        UpdateAABB();
    }
};

struct SpriteRenderData
{
    Texture2D sourceTexture = {};
    
    Vector2 position = {0,0};
    
    float scale = 1;

    //animation
    std::vector<Rectangle> animationFrames = {};

    int startFrame = 0;
    int endFrame = (int)animationFrames.size();

    int spacing = 0;

    int maxFrames = 1;
    Vector2 frameSize = {1,1};
};

struct EntityData
{
    bool flipX = false;
    bool flipY = false;
};

inline void UpdateSprite(
    SpriteRenderData* renderData,
    const EntityData& entityData,
    const Vector2& position,
    Vector2 offset
)
{
    float offsetX = offset.x;
    float offsetY = offset.y;

    if(entityData.flipX) offsetX = -offset.x;

    if(entityData.flipY) offsetY = -offset.y;

    renderData->position = {position.x + offsetX, position.y + offsetY};
}