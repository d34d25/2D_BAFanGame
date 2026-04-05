#pragma once

#include "entity.h"

void SolveCollisions(GameObject* objA, GameObject* objB, bool isX, bool gravityUp, bool isTrampoline);

void SolveCollisions_Platform(GameObject* objA, GameObject* objB, bool isX);

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

inline void SolveCollisionsOneWayLeftRight(GameObject * objA, GameObject* objB, bool isRight)
{
    bool isLeft = !isRight;

    float offset = 6.0f;

    if(isLeft) if(IsLeft(objA->aabb, objB->aabb, offset) || objA->body.velocity.x <= 0.0f) return;

    if(isRight) if(IsRight(objA->aabb, objB->aabb, offset) || objA->body.velocity.x >= 0.0f) return;

    SolveCollisions(objA, objB, true, false, false);
}

inline void SolveCollisionsOneWayUpDown(GameObject * objA, GameObject* objB, bool isUp ,bool gravityUp, bool isPlatform)
{
    bool isDown = !isUp;

    float offset = 15.0f;

    if(!isPlatform)
    {
        if(isUp) if(!IsAbove(objA->aabb, objB->aabb, offset) || objA->body.velocity.y <= 0.0f) return;

        if(isDown) if(!IsBelow(objA->aabb, objB->aabb, offset) || objA->body.velocity.y >= 0.0f) return;
    }
    else
    {
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
    }    

    SolveCollisions(objA, objB, false, gravityUp, false);
}