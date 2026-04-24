#include "physics.h"

void SolveCollisions(GameObject *objA, GameObject *objB, bool isX, bool gravityUp, bool isTrampoline, bool isPlatform)
{
    if(!CheckCollisionRecs(*objA->GetMainAABB(), *objB->GetMainAABB())) return;

    float overlap = 0;

    float offset = 0.001f;

    float trampolineImpulse = 1800.0f;

    float trampolineImpulseFactor = 0.6f;

    if(isX)
    {
        if(objA->GetMainAABB()->x <= objB->GetMainAABB()->x) // A is at the left
        {
            overlap = (objA->GetMainAABB()->x + objA->GetMainAABB()->width) - objB->GetMainAABB()->x;
            objA->position.x -= (overlap + offset);

            if(objA->body.velocity.x > 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = -trampolineImpulse * trampolineImpulseFactor;
        }
        else // A is at the right
        {
            overlap = (objB->GetMainAABB()->x + objB->GetMainAABB()->width) - objA->GetMainAABB()->x;
            objA->position.x += (overlap + offset);

            if(objA->body.velocity.x < 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = trampolineImpulse * trampolineImpulseFactor;
        }
    }
    else
    {
        if(objA->GetMainAABB()->y <= objB->GetMainAABB()->y) // A is above of B
        {
            overlap = (objA->GetMainAABB()->y + objA->GetMainAABB()->height) - objB->GetMainAABB()->y;
            objA->position.y -= (overlap + offset);

            if(!gravityUp && isPlatform) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y > 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = -trampolineImpulse;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = -trampolineImpulse * trampolineImpulseFactor;
        }
        else //A is below B
        {
            overlap = (objB->GetMainAABB()->y + objB->GetMainAABB()->height) - objA->GetMainAABB()->y;
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
    if(!CheckCollisionRecs(*objA->GetMainAABB(), *objB->GetMainAABB())) return;

    float offset = 0.001f;

    if(isX)
    {
        if(objA->GetMainAABB()->x <= objB->GetMainAABB()->x)
        {
            objA->GetMainAABB()->x = (objB->GetMainAABB()->x - objA->GetMainAABB()->width * 0.5f) - offset;
        }
        else
        {
            objA->position.x = ((objB->GetMainAABB()->x + objB->GetMainAABB()->width) + objA->GetMainAABB()->width * 0.5f) + offset;
        }

        objA->body.velocity.x *= -1;
    }
    else
    {
        if(objA->GetMainAABB()->y <= objB->GetMainAABB()->y)
        {
            objA->position.y = (objB->GetMainAABB()->y - objA->GetMainAABB()->height * 0.5f) - offset;
        }
        else
        {
            objA->position.y = ((objB->GetMainAABB()->y + objB->GetMainAABB()->height) + objA->GetMainAABB()->height * 0.5f) + offset;
        }

        objA->body.velocity.y *= -1;
    }

    objA->UpdateAABB();
}
