#pragma once

#include "raylib.h"
#include "raymath.h"

#include "definitions.h"

#include <vector>
#include <iostream>

const float MASS = 1;

const Vector2 MAX_SPEED = {600,600};

struct Transform2D
{
    Vector2 position = {0,0};

    float scale = tileScale;

    float angle = 0.0f; //in degrees
};

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
};

struct GameObject
{
    Transform2D transform = {};

    std::vector<Hitbox> hitboxes = {};

    SimpleBody2D* body = nullptr;

    bool canEntityCollidePhysically = false;
    bool canPlatformCollidePhysically = false;

    GameObject() = default;

    ~GameObject();

    inline void UpdateHitboxes()
    {
        if(hitboxes.empty()) return;

        for(int i = 0; i < hitboxes.size(); i++)
        {
            hitboxes[i].Update(transform);
        }
    }

    inline Rectangle* GetMainAABB()
    {
        return &hitboxes[0].aabb;
    }

    inline Rectangle* GetSubAABB(int index)
    {
        if(index < 1) return &hitboxes[1].aabb;

        return &hitboxes[index].aabb;
    }

    inline void AddSubHitbox(Vector2 offset, Vector2 size)
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
        if(!body) return;

        body->UpdateVelocity(dt, iterations, gravity);
    }

    inline void UpdatePositionX(float dt, int iterations)
    {
        if(!body) return;

        body->UpdatePositionX(dt, iterations, &transform.position.x);
        
        UpdateHitboxes();
    }

    inline void UpdatePositionY(float dt, int iterations)
    {
        if(!body) return;

        body->UpdatePositionY(dt, iterations, &transform.position.y);
        
        UpdateHitboxes();
    }
};

struct SpriteRenderData
{
    Texture2D* sourceTexture = nullptr;

    Vector2 offset = {0,0};

    //animation
    std::vector<Rectangle> animationFrames = {};

    int spacing = 0;

    int maxFrames = 1;

    float animationSpeed = 5.0f;
};

struct EntityData
{
    bool flipX = false;
    bool flipY = false;
};