#pragma once

#include "raylib.h"
#include "raymath.h"

struct GameObject
{
private:

    Vector2 finalVelocity = {0,0};

public:

    Vector2 position = {0,0};

    Vector2 velocity = {0,0};
    Vector2 altVelocity = {0,0};

    float scale  = 0;
    float rotation = 0;

    Rectangle aabb = {0,0,50,50};

    bool canEntityCollide = false;
    bool canPlatformCollide = false;

    void UpdateAABB()
    {
        aabb.x = position.x - aabb.width * 0.5f;
        aabb.y = position.y - aabb.height * 0.5f;
    }

    void UpdateVelocity()
    {
        finalVelocity = Vector2Add(velocity, altVelocity);
    }

    void UpdatePositionX(float dt)
    {
        position.x += finalVelocity.x * dt;
        UpdateAABB();
    }

    void UpdatePositionY(float dt)
    {
        position.y += finalVelocity.y * dt;
        UpdateAABB();
    }
};

struct SpriteRenderData
{
    Texture2D texture = {};
    Rectangle sourceRect = {0,0,1,1};
    Vector2 position = {0,0};
    float scale = 1;
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