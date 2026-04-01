#pragma once

#include "entity.h"
#include <iostream>

inline bool IsLeft(const Rectangle& aabb_A, const Rectangle& aabb_B, float offset)
{
    float right = aabb_A.x + aabb_A.width;
    float left = aabb_B.x;

    return (right > left + offset);
}

inline bool IsRight(const Rectangle& aabb_A, const Rectangle& aabb_B, float offset)
{
    float right = aabb_B.x + aabb_B.width;
    float left = aabb_A.x;

    return (left < right - offset);
}

inline bool IsAbove(const Rectangle& aabb_A, const Rectangle& aabb_B, float offset)
{
    float bottom = aabb_A.y + aabb_A.height;
    float top = aabb_B.y;

    return (bottom < top + offset);
}

inline bool IsBelow(const Rectangle& aabb_A, const Rectangle& aabb_B, float offset)
{
    float bottom = aabb_B.y + aabb_B.height;
    float top = aabb_A.y;

    return (top > bottom - offset);
}

inline void SolveCollisions(GameObject* objA, GameObject* objB, bool isX, bool gravityUp)
{
    if(!CheckCollisionRecs(objA->aabb, objB->aabb)) return;

    float overlap = 0;

    float offset = 0.001f;

    if(isX)
    {
        if(objA->aabb.x <= objB->aabb.x) // A is at the left
        {
            overlap = (objA->aabb.x + objA->aabb.width) - objB->aabb.x;
            objA->position.x -= (overlap + offset);

            if(objA->body.velocity.x > 0) objA->body.velocity.x = 0;
        }
        else // A is at the right
        {
            overlap = (objB->aabb.x + objB->aabb.width) - objA->aabb.x;
            objA->position.x += (overlap + offset);

            if(objA->body.velocity.x < 0) objA->body.velocity.x = 0;
        }
    }
    else
    {
        if(objA->aabb.y <= objB->aabb.y) // A is above of B
        {
            overlap = (objA->aabb.y + objA->aabb.height) - objB->aabb.y;
            objA->position.y -= (overlap + offset);

            if(!gravityUp) objA->body.altVelocity = objB->body.velocity;
        }
        else //A is below B
        {
            overlap = (objB->aabb.y + objB->aabb.height) - objA->aabb.y;
            objA->position.y += (overlap + offset);

            if(gravityUp) objA->body.altVelocity = objB->body.velocity;
        }

        objA->body.velocity.y = 0;
    }

    objA->UpdateAABB();
}


inline void SolveCollisionsOneWayLeftRight(GameObject * objA, GameObject* objB, bool isRight)
{
    bool isLeft = !isRight;

    float offset = 6.0f;

    if(isLeft) if(IsLeft(objA->aabb, objB->aabb, offset) || objA->body.velocity.x <= 0.0f) return;

    if(isRight) if(IsRight(objA->aabb, objB->aabb, offset) || objA->body.velocity.x >= 0.0f) return;

    SolveCollisions(objA, objB, true, false);
}

inline void SolveCollisionsOneWayUpDown(GameObject * objA, GameObject* objB, bool isUp ,bool gravityUp)
{
    bool isDown = !isUp;

    float offset = 15.0f;

    if(!gravityUp)
    {
        if(isUp) if(!IsAbove(objA->aabb, objB->aabb, offset) || objA->body.velocity.y <= 0.0f) return;

        if(isDown) if(!IsBelow(objA->aabb, objB->aabb, offset) || objA->body.velocity.y >= 0.0f) return;
    }
    else
    {
        if(isDown) if(!IsAbove(objA->aabb, objB->aabb, offset) || objA->body.velocity.y <= 0.0f) return;

        if(isUp) if(!IsBelow(objA->aabb, objB->aabb, offset) || objA->body.velocity.y >= 0.0f) return;
    }

    SolveCollisions(objA, objB, false, gravityUp);
}

inline void SolveCollisions_Platform(GameObject* objA, GameObject* objB, bool isX)
{
    if(!CheckCollisionRecs(objA->aabb, objB->aabb)) return;

    float offset = 0.001f;

    if(isX)
    {
        if(objA->aabb.x <= objB->aabb.x)
        {
            objA->position.x = (objB->aabb.x - objA->aabb.width * 0.5f) - offset;
        }
        else
        {
            objA->position.x = ((objB->aabb.x + objB->aabb.width) + objA->aabb.width * 0.5f) + offset;
        }

        objA->body.velocity.x *= -1;
    }
    else
    {
        if(objA->aabb.y <= objB->aabb.y)
        {
            objA->position.y = (objB->aabb.y - objA->aabb.height * 0.5f) - offset;
        }
        else
        {
            objA->position.y = ((objB->aabb.y + objB->aabb.height) + objA->aabb.height * 0.5f) + offset;
        }

        objA->body.velocity.y *= -1;
    }

    objA->UpdateAABB();
}