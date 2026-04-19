#include "physics.h"

void SolveCollisions(GameObject *objA, GameObject *objB, bool isX, bool gravityUp, bool isTrampoline, bool isPlatform)
{
    if(!CheckCollisionRecs(objA->mainAABB, objB->mainAABB)) return;

    float overlap = 0;

    float offset = 0.001f;

    float trampolineImpulse = 1800.0f;

    float trampolineImpulseFactor = 0.6f;

    if(isX)
    {
        if(objA->mainAABB.x <= objB->mainAABB.x) // A is at the left
        {
            overlap = (objA->mainAABB.x + objA->mainAABB.width) - objB->mainAABB.x;
            objA->position.x -= (overlap + offset);

            if(objA->body.velocity.x > 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = -trampolineImpulse * trampolineImpulseFactor;
        }
        else // A is at the right
        {
            overlap = (objB->mainAABB.x + objB->mainAABB.width) - objA->mainAABB.x;
            objA->position.x += (overlap + offset);

            if(objA->body.velocity.x < 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = trampolineImpulse * trampolineImpulseFactor;
        }
    }
    else
    {
        if(objA->mainAABB.y <= objB->mainAABB.y) // A is above of B
        {
            overlap = (objA->mainAABB.y + objA->mainAABB.height) - objB->mainAABB.y;
            objA->position.y -= (overlap + offset);

            if(!gravityUp && isPlatform) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y > 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = -trampolineImpulse;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = -trampolineImpulse * trampolineImpulseFactor;
        }
        else //A is below B
        {
            overlap = (objB->mainAABB.y + objB->mainAABB.height) - objA->mainAABB.y;
            objA->position.y += (overlap + offset);

            if(gravityUp && isPlatform) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y < 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = trampolineImpulse * trampolineImpulseFactor;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = trampolineImpulse;
        }
    }

    objA->UpdateAABB();
}

void SolveCollisions_Platform(GameObject *objA, GameObject *objB, bool isX)
{
    if(!CheckCollisionRecs(objA->mainAABB, objB->mainAABB)) return;

    float offset = 0.001f;

    if(isX)
    {
        if(objA->mainAABB.x <= objB->mainAABB.x)
        {
            objA->position.x = (objB->mainAABB.x - objA->mainAABB.width * 0.5f) - offset;
        }
        else
        {
            objA->position.x = ((objB->mainAABB.x + objB->mainAABB.width) + objA->mainAABB.width * 0.5f) + offset;
        }

        objA->body.velocity.x *= -1;
    }
    else
    {
        if(objA->mainAABB.y <= objB->mainAABB.y)
        {
            objA->position.y = (objB->mainAABB.y - objA->mainAABB.height * 0.5f) - offset;
        }
        else
        {
            objA->position.y = ((objB->mainAABB.y + objB->mainAABB.height) + objA->mainAABB.height * 0.5f) + offset;
        }

        objA->body.velocity.y *= -1;
    }

    objA->UpdateAABB();
}
