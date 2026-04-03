#include "physics.h"

void SolveCollisions(GameObject *objA, GameObject *objB, bool isX, bool gravityUp, bool isTrampoline)
{
    if(!CheckCollisionRecs(objA->aabb, objB->aabb)) return;

    float overlap = 0;

    float offset = 0.001f;

    float trampolineImpulse = 1800.0f;

    float trampolineImpulseFactor = 0.6f;

    if(isX)
    {
        if(objA->aabb.x <= objB->aabb.x) // A is at the left
        {
            overlap = (objA->aabb.x + objA->aabb.width) - objB->aabb.x;
            objA->position.x -= (overlap + offset);

            if(objA->body.velocity.x > 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = -trampolineImpulse * trampolineImpulseFactor;
        }
        else // A is at the right
        {
            overlap = (objB->aabb.x + objB->aabb.width) - objA->aabb.x;
            objA->position.x += (overlap + offset);

            if(objA->body.velocity.x < 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = trampolineImpulse * trampolineImpulseFactor;
        }
    }
    else
    {
        if(objA->aabb.y <= objB->aabb.y) // A is above of B
        {
            overlap = (objA->aabb.y + objA->aabb.height) - objB->aabb.y;
            objA->position.y -= (overlap + offset);

            if(!gravityUp) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y > 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = -trampolineImpulse;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = -trampolineImpulse * trampolineImpulseFactor;
        }
        else //A is below B
        {
            overlap = (objB->aabb.y + objB->aabb.height) - objA->aabb.y;
            objA->position.y += (overlap + offset);

            if(gravityUp) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y < 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = trampolineImpulse * trampolineImpulseFactor;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = trampolineImpulse;
        }
    }

    objA->UpdateAABB();
}

void SolveCollisions_Platform(GameObject *objA, GameObject *objB, bool isX)
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
