#include "physics.h"

void SolveCollisions(GameObject *objA, GameObject *objB, bool isX, bool gravityUp, bool isTrampoline, bool isPlatform)
{
    //if(!CheckCollisionRecs(objA->GetMainAABB(), objB->GetMainAABB())) return;

    float overlap = 0;

    float offset = 0.001f;

    float trampolineImpulse = 1800.0f;

    float trampolineImpulseFactor = 0.6f;

    float trampolineFactor_Y = 3.0f;

    if(isX)
    {
        if(objA->GetMainAABB().x <= objB->GetMainAABB().x) // A is at the left
        {
            overlap = (objA->GetMainAABB().x + objA->GetMainAABB().width) - objB->GetMainAABB().x;
            objA->transform.position.x -= (overlap + offset);

            if(objA->body.velocity.x > 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = -trampolineImpulse * trampolineImpulseFactor;
        }
        else // A is at the right
        {
            overlap = (objB->GetMainAABB().x + objB->GetMainAABB().width) - objA->GetMainAABB().x;
            objA->transform.position.x += (overlap + offset);

            if(objA->body.velocity.x < 0) objA->body.velocity.x = 0;

            if(isTrampoline) objA->body.velocity.x = trampolineImpulse * trampolineImpulseFactor;
        }
    }
    else
    {
        if(objA->GetMainAABB().y <= objB->GetMainAABB().y) // A is above of B
        {
            overlap = (objA->GetMainAABB().y + objA->GetMainAABB().height) - objB->GetMainAABB().y;
            objA->transform.position.y -= (overlap + offset);

            if(!gravityUp && isPlatform) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y > 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = -trampolineImpulse * trampolineFactor_Y;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = -trampolineImpulse * trampolineImpulseFactor;
        }
        else //A is below B
        {
            overlap = (objB->GetMainAABB().y + objB->GetMainAABB().height) - objA->GetMainAABB().y;
            objA->transform.position.y += (overlap + offset);

            if(gravityUp && isPlatform) objA->body.altVelocity = objB->body.velocity;

            if(objA->body.velocity.y < 0) objA->body.velocity.y = 0;

            if(isTrampoline && !gravityUp) objA->body.velocity.y = trampolineImpulse * trampolineImpulseFactor;
            else if(isTrampoline && gravityUp) objA->body.velocity.y = trampolineImpulse * trampolineFactor_Y;
        }
    }

    objA->UpdateHitboxes();
}

void SolveCollisions_Platform(GameObject *objA, GameObject *objB, bool isX)
{
    //if(!CheckCollisionRecs(objA->GetMainAABB(), objB->GetMainAABB())) return;

    float offset = 0.001f;

    if(isX)
    {
        if(objA->GetMainAABB().x <= objB->GetMainAABB().x)
        {
            objA->GetMainAABB().x = (objB->GetMainAABB().x - objA->GetMainAABB().width * 0.5f) - offset;
        }
        else
        {
            objA->transform.position.x = ((objB->GetMainAABB().x + objB->GetMainAABB().width) + objA->GetMainAABB().width * 0.5f) + offset;
        }

        objA->body.velocity.x *= -1;
    }
    else
    {
        if(objA->GetMainAABB().y <= objB->GetMainAABB().y)
        {
            objA->transform.position.y = (objB->GetMainAABB().y - objA->GetMainAABB().height * 0.5f) - offset;
        }
        else
        {
            objA->transform.position.y = ((objB->GetMainAABB().y + objB->GetMainAABB().height) + objA->GetMainAABB().height * 0.5f) + offset;
        }

        objA->body.velocity.y *= -1;
    }

    objA->UpdateHitboxes();
}

CCD_CollisionResult CheckCollisionsBulletVsEntity_CCD(Bullet *bullet, GameObject *target, float dt)
{
    CCD_CollisionResult result = {};

    Vector2 relativeVelocity = Vector2Scale(Vector2Subtract(target->body.GetFinalVelocity(), bullet->velocity), dt);

    float minSpeed = 0.00001f;

    if((fabs(relativeVelocity.x) < minSpeed) && (fabs(relativeVelocity.y) < minSpeed)) return result;

    Rectangle aabb = target->GetMainAABB();

    float minX = aabb.x - bullet->radius;
    float maxX = aabb.x + aabb.width + bullet->radius;

    float minY = aabb.y - bullet->radius;
    float maxY = aabb.y + aabb.height + bullet->radius;

    double frameStart = 0.0;
    double frameEnd = 1.0;
    
    if(fabs(relativeVelocity.x) < minSpeed)
    {
        if(bullet->posititon.x < minX || bullet->posititon.x > maxX)
            return result;
    }
    else
    {
        double entryTime = (minX - bullet->posititon.x) / relativeVelocity.x;
        double exitTime = (maxX - bullet->posititon.x) / relativeVelocity.x;

        if(entryTime > exitTime) std::swap(entryTime, exitTime);

        if(entryTime > 1.0 || exitTime < 0.0) return result;

        entryTime = std::max(entryTime, 0.0);
        exitTime = std::min(exitTime, 1.0);

        if(entryTime > exitTime) return result;

        frameStart = std::max(frameStart, entryTime);
        frameEnd = std::min(frameEnd, exitTime);

        if(frameStart > frameEnd) return result;
    }

    if(fabs(relativeVelocity.y) < minSpeed)
    {
        if(bullet->posititon.y < minY || bullet->posititon.y > maxY)
            return result;
    }
    else
    {
        double entryTime = (minY - bullet->posititon.y) / relativeVelocity.y;
        double exitTime = (maxY - bullet->posititon.y) / relativeVelocity.y;

        if(entryTime > exitTime) std::swap(entryTime, exitTime);

        if(entryTime > 1.0 || exitTime < 0.0) return result;

        entryTime = std::max(entryTime, 0.0);
        exitTime = std::min(exitTime, 1.0);

        if(entryTime > exitTime) return result;

        frameStart = std::max(frameStart, entryTime);
        frameEnd = std::min(frameEnd, exitTime);

        if(frameStart > frameEnd) return result;
    }

    float hitX = bullet->posititon.x + relativeVelocity.x * frameStart;
    float hitY = bullet->posititon.y + relativeVelocity.y * frameStart;

    float closestX = Clamp(hitX, aabb.x, aabb.x + aabb.width);
    float closestY = Clamp(hitY, aabb.y, aabb.y + aabb.height);

    float distX = hitX - closestX;
    float distY = hitY - closestY;
    
    float distanceSquared = (distX * distX) + (distY * distY);

    if(distanceSquared > bullet->radius * bullet->radius) return result;

    result.collision = true;
    result.entryTime = frameStart;
    result.exitTime = frameEnd;

    return result;
}
