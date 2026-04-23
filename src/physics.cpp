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

void SolveCollisions_CCD_AxisSplit(GameObject *objA, GameObject *objB, bool isX, float dt)
{
    if(isX)
    {
        if(objA->mainAABB.y + objA->mainAABB.height <= objB->mainAABB.y ||
        objA->mainAABB.y >= objB->mainAABB.y + objB->mainAABB.height) return;
    }
    else
    {
        if(objA->mainAABB.x + objA->mainAABB.width <= objB->mainAABB.x ||
        objA->mainAABB.x >= objB->mainAABB.x + objB->mainAABB.width) return;
    }

    double frameStart = 0.0;
    double frameEnd = 1.0;

    float velocityA = isX ? objA->body.velocity.x : objA->body.velocity.y;
    float velocityB = isX ? objB->body.velocity.x : objB->body.velocity.y;

    double relativeSpeed = (velocityB - velocityA) * dt;

    if(fabs(relativeSpeed) < 0.0000001f) return;

    float minA = isX ? objA->mainAABB.x : objA->mainAABB.y;
    float maxA = isX ? objA->mainAABB.x + objA->mainAABB.width : objA->mainAABB.y + objA->mainAABB.height;

    float minB = isX ? objB->mainAABB.x : objB->mainAABB.y;
    float maxB = isX ? objB->mainAABB.x + objB->mainAABB.width : objB->mainAABB.y + objB->mainAABB.height;

    double entryTime = (minA - maxB) / relativeSpeed; 
    double exitTime = (maxA - minB) / relativeSpeed;

    if(entryTime > exitTime) std::swap(entryTime, exitTime);

    if(entryTime > 1.0 || exitTime < 0.0) return;

    entryTime = std::max(entryTime, 0.0);
    exitTime = std::min(exitTime, 1.0);

    if(entryTime > exitTime) return;

    frameStart = std::max(frameStart, entryTime);
    frameEnd = std::min(frameEnd, exitTime);

    if(frameStart > frameEnd) return;

    float currentVel = isX ? objA->body.velocity.x : objA->body.velocity.y;

    if(fabs(currentVel) < 0.0001f) return;

    //resolution

    float offset = 0.5f;

    float halfSizeA = isX ? objA->mainAABB.width * 0.5f : objA->mainAABB.height * 0.5f;
    float halfSizeB = isX ? objB->mainAABB.width * 0.5f : objB->mainAABB.height * 0.5f;

    if(isX)
    {
        if(velocityA > velocityB)
        {
            objA->position.x = objB->mainAABB.x - halfSizeA - offset;
        }
        else
        {
            objA->position.x = (objB->mainAABB.x + objB->mainAABB.width) + halfSizeA + offset;
        }

        objA->body.velocity.x = 0;
    }
    else
    {
        if(velocityA > velocityB)
        {
            objA->position.y = objB->mainAABB.y - halfSizeA - offset;
        }
        else
        {
            objA->position.y = (objB->mainAABB.y + objB->mainAABB.height) + halfSizeA + offset;
        }

        objA->body.velocity.y = objB->body.velocity.y;

        objA->body.altVelocity.x = objB->body.velocity.x;
    }

    objA->UpdateAABB();
}
