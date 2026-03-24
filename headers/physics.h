#pragma once

#include "entity.h"

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

            if(objA->velocity.x > 0) objA->velocity.x = 0;
        }
        else // A is at the right
        {
            overlap = (objB->aabb.x + objB->aabb.width) - objA->aabb.x;
            objA->position.x += (overlap + offset);

            if(objA->velocity.x < 0) objA->velocity.x = 0;
        }
    }
    else
    {
        if(objA->aabb.y <= objB->aabb.y) // A is above of B
        {
            overlap = (objA->aabb.y + objA->aabb.height) - objB->aabb.y;
            objA->position.y -= (overlap + offset);

            if(!gravityUp) objA->altVelocity = objB->velocity;
        }
        else //A is below B
        {
            overlap = (objB->aabb.y + objB->aabb.height) - objA->aabb.y;
            objA->position.y += (overlap + offset);

            if(gravityUp) objA->altVelocity = objB->velocity;
        }

        objA->velocity.y = 0;
    }

    objA->UpdateAABB();
}