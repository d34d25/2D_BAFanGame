#pragma once

#include "raylib.h"
#include "raymath.h"

#include <vector>

const float MASS = 1;

const Vector2 MAX_SPEED = {600,600};

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

        finalVelocity.x = Clamp(finalVelocity.x, -MAX_SPEED.x, MAX_SPEED.x);

        finalVelocity.y = Clamp(finalVelocity.y, -MAX_SPEED.y, MAX_SPEED.y);
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

struct Hitbox
{
    Rectangle aabb = {0,0,0,0};
    
    Vector2 offset = {0,0};

    Hitbox(Vector2 position, Vector2 size)
    {
        aabb = {
            position.x,
            position.y,
            size.x,
            size.y
        };
    }

    Hitbox(Vector2 position, Vector2 offset, Vector2 size)
    {
        this->offset = offset;

        aabb = {
            position.x + this->offset.x,
            position.y + this->offset.y,
            size.x,
            size.y
        };
    }
};

struct GameObject
{

public:

    Vector2 position = {0,0};

    Hitbox mainHitbox = {{0,0},{0,0}};

    //these secondary hitboxes are only used for triggers (when necessary), not physics
    std::vector<Hitbox> subHitboxList = {};

    float scale  = 0;

    SimpleBody2D body = {};

    bool canEntityCollidePhysically = false;
    bool canPlatformCollidePhysically = false;

    inline Rectangle* GetMainAABB()
    {
        return &mainHitbox.aabb;
    }

    inline Rectangle* GetSubAABB(int index)
    {
        return &subHitboxList[index].aabb;
    }

    inline void AddSubHitbox(Vector2 offset, Vector2 size)
    {
        Vector2 spawnPosition = {
            position.x - (size.x * 0.5f),
            position.y - (size.y * 0.5f)
        };

        subHitboxList.push_back(Hitbox{spawnPosition, offset, size});
    }

    inline void UpdateAABB()
    {
        GetMainAABB()->x = position.x - GetMainAABB()->width * 0.5f;
        GetMainAABB()->y = position.y - GetMainAABB()->height * 0.5f;

        for(int i = 0; i < subHitboxList.size(); i++)
        {
           GetSubAABB(i)->x = (position.x - GetSubAABB(i)->width * 0.5f) + subHitboxList[i].offset.x;
           GetSubAABB(i)->y = (position.y - GetSubAABB(i)->height * 0.5f) + subHitboxList[i].offset.y;
        }
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

    float animationSpeed = 5.0f;
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