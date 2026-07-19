#pragma once

#include "raylib.h"
#include "raymath.h"

#include "definitions.h"

#include <vector>
#include <iostream>

const float MASS = 1;

/*
max relative velocity 1750

with a ONE_WAY_TOLERANCE of 3 (the thinnest collider's width / height) 
and 10 iterations with a fixed 60fps delta time 

this means that the combined velocity of
both objects involved in the collision has to
add up to 1750 at maximum

if both objects are moving at the same speed 
in opposite directions the max speed for each
one would be 1750 / 2 = 875 
*/

const Vector2 MAX_SPEED = {600,600};

enum struct Direction
{
    UP, RIGHT, DOWN, LEFT
};

struct Transform2D
{
    Vector2 position = {0,0};

    float angle = 0.0f; //in degrees
};

struct SimpleBody2D
{
    Vector2 velocity = {0,0};

    Vector2 altVelocity = {0,0};

    Vector2 finalVelocity = {0,0};

    Vector2 acceleration = {0,0};

    Vector2 force = {0,0};

    float damping = 7.0f; //4.0f

    bool hasGravity = false;

    inline void AddVelocities()
    {
        finalVelocity = Vector2Add(velocity, altVelocity);

        finalVelocity.x = Clamp(finalVelocity.x, -MAX_SPEED.x, MAX_SPEED.x);

        finalVelocity.y = Clamp(finalVelocity.y, -MAX_SPEED.y, MAX_SPEED.y);
    }

    void UpdateVelocity(float dt, int iterations, float gravity);

    inline void UpdatePositionX(float dt, int iterations, float& xpos)
    {
        float subDt = dt / iterations;

        xpos += finalVelocity.x * subDt;
    }

    inline void UpdatePositionY(float dt, int iterations, float& ypos)
    {
        float subDt = dt / iterations;

        ypos += finalVelocity.y * subDt;
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

    Hitbox(Vector2 position, Vector2 size, Vector2 offset = {0,0}, bool centered = false)
    {
        this->offset = offset;

        if(!centered)
        {
            aabb = {
                position.x + this->offset.x,
                position.y + this->offset.y,
                size.x,
                size.y
            };
        }
        else
        {
            aabb = {
                (position.x - (size.x * 0.5f)) + this->offset.x,
                (position.y - (size.y * 0.5f)) + this->offset.y,
                size.x,
                size.y
            };
        }
    }

    inline void Update(const Transform2D& transform)
    {
        aabb.x = (transform.position.x - aabb.width * 0.5f) + offset.x;
        aabb.y = (transform.position.y - aabb.height * 0.5f) + offset.y;
    }

    inline void Update(const Vector2& position)
    {
        aabb.x = (position.x - aabb.width * 0.5f) + offset.x;
        aabb.y = (position.y - aabb.height * 0.5f) + offset.y;
    }
};

struct SpriteFlipData
{
    bool flipX = false;
    bool flipY = false;

    bool flipOffset = false;
};

struct GameObject
{
    SimpleBody2D body = {};

    std::vector<Hitbox> hitboxes = {};

    Transform2D transform = {};

    SpriteFlipData flipData = {};  

    GameObject() = default;

    ~GameObject() = default;

    inline void UpdateHitboxes()
    {
        if(hitboxes.empty()) return;

        for(int i = 0; i < hitboxes.size(); i++)
        {
            hitboxes[i].Update(transform);
        }
    }

    inline Rectangle& GetMainAABB()
    {
        return hitboxes[0].aabb;
    }

    inline Rectangle& GetSubAABB(int index)
    {
        if(index < 1) return hitboxes[1].aabb;

        return hitboxes[index].aabb;
    }

    inline Rectangle& GetAABB(int index)
    {
        return hitboxes[index].aabb;
    }

    inline void AddSubHitbox(const Vector2& offset, const Vector2& size)
    {
        if(hitboxes.size() >= 1)
        {
            hitboxes.push_back(Hitbox{transform.position, size, offset, true});    
        }
        else
        {
            std::cout<<"THERE IS NO MAIN HITBOX"<<"\n";
        }
    }

    inline void UpdateVelocity(float dt, int iterations, float gravity)
    {
        body.UpdateVelocity(dt, iterations, gravity);
    }

    inline void UpdatePositionX(float dt, int iterations)
    {
        body.UpdatePositionX(dt, iterations, transform.position.x);
        
        UpdateHitboxes();
    }

    inline void UpdatePositionY(float dt, int iterations)
    {
        body.UpdatePositionY(dt, iterations, transform.position.y);
        
        UpdateHitboxes();
    }
};

struct SpriteRenderData
{
    std::vector<Rectangle> animationFrames = {};

    Texture2D* sourceTexture = nullptr;

    Vector2 offset = {0,0};

    Vector2 ogOffset = {0,0};

    Vector2 frameSize = {0,0};

    float animationSpeed = 5.0f;

    int spacing = 0; //used for time based animations (passive animations)

    int maxFrames = 1;
};

inline Direction CalculateDirection(float angle, SpriteFlipData data)
{
    //un-flipped

    if(angle == 0 && !data.flipY) return Direction::UP;

    if(angle == 90 && !data.flipX) return Direction::RIGHT;

    if(angle == 180 && !data.flipY) return Direction::DOWN;

    if(angle == 270 && !data.flipX) return Direction::LEFT;

    //fliped

    if(angle == 0 && data.flipY) return Direction::DOWN;

    if(angle == 90 && data.flipX) return Direction::LEFT;

    if(angle == 180 && data.flipY) return Direction::UP;

    if(angle == 270 && data.flipX) return Direction::RIGHT;

    return Direction::UP;
};

inline const char* GetDirectionText(Direction direction)
{
    switch (direction)
    {
    case Direction::UP: return "UP";
    case Direction::RIGHT: return "RIGHT";
    case Direction::DOWN: return "DOWN";
    case Direction::LEFT: return "LEFT";
    
    default: return "UNDEFINED DIRECTION";
    }
};

inline void FlipHitboxY(Hitbox& hitbox, bool flipY, bool invert)
{
    float offset = std::abs(hitbox.offset.y);

    if(flipY)
    {
        hitbox.offset.y = invert ? offset : -offset;
    }
    else
    {
        hitbox.offset.y = invert ? -offset : offset;
    }
};

inline Vector2 GetTextureBulletSpawnPos(const GameObject& gameObj, const SpriteRenderData* renderData)
{
    Vector2 spawnPos = gameObj.transform.position;

    if(!renderData) return spawnPos;

    Vector2 offset = renderData->offset;

    int textureWidth = renderData->frameSize.x;

    if(gameObj.flipData.flipX)
    {
        offset.x = -offset.x;

        textureWidth = -textureWidth;
    }

    if(gameObj.flipData.flipY)
    {
        offset.y = -offset.y;
    }

    spawnPos.x += offset.x + textureWidth;
    spawnPos.y += offset.y;

    return spawnPos;
};