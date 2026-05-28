#pragma once

#include "entity.h"
#include "bullet.h"

#include <iostream>

struct CCD_CollisionResult
{
    bool collision = false;

    float entryTime = 0.0f;
    float exitTime = 0.0f;
};

void SolveCollisions(GameObject* objA, GameObject* objB, bool isX, bool gravityUp, bool isTrampoline, bool isPlatform);

void SolveCollisions_Platform(GameObject* objA, GameObject* objB, bool isX);

CCD_CollisionResult CheckCollisionsBulletVsEntity_CCD(Bullet* bullet, GameObject* target, float dt);

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

    if(isLeft) if(IsLeft(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.x <= 0.0f) return;

    if(isRight) if(IsRight(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.x >= 0.0f) return;

    SolveCollisions(objA, objB, true, false, false, false);
}

inline void SolveCollisionsOneWayUpDown(GameObject * objA, GameObject* objB, bool isUp ,bool gravityUp, bool isPlatform)
{
    bool isDown = !isUp;

    float offset = 15.0f;

    if(!isPlatform)
    {
        if(isUp) if(!IsAbove(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y <= 0.0f) return;

        if(isDown) if(!IsBelow(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y >= 0.0f) return;
    }
    else
    {
        if(!gravityUp)
        {
            if(isUp) if(!IsAbove(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y <= 0.0f) return;

            if(isDown) if(!IsBelow(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y >= 0.0f) return;
        }
        else
        {
            if(isDown) if(!IsAbove(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y <= 0.0f) return;

            if(isUp) if(!IsBelow(objA->GetMainAABB(), objB->GetMainAABB(), offset) || objA->body.velocity.y >= 0.0f) return;
        }
    }    

    SolveCollisions(objA, objB, false, gravityUp, false, isPlatform);
}

inline void ApplyWind(
    GameObject* objA, GameObject* objB,
    Direction direction, 
    bool isEdgeUp, bool isEdgeDown,
    bool gravityUp
)
{ 
    bool up = direction == Direction::UP;
    
    bool down = direction == Direction::DOWN;

    bool left = direction == Direction::LEFT;

    bool right = direction == Direction::RIGHT;

    float windForce = 1600 * objA->body.damping;

    bool isEdge = (!gravityUp && isEdgeUp) || (gravityUp && isEdgeDown);

    float offset = objB->GetMainAABB().height * 0.8f;

    bool above = IsAbove(objA->GetMainAABB(), objB->GetMainAABB(), offset);

    bool below = IsBelow(objA->GetMainAABB(), objB->GetMainAABB(), offset);

    bool onTop = !gravityUp ? above : below;

    bool falling = !gravityUp ? objA->body.velocity.y > 0 : objA->body.velocity.y < 0;

    if(isEdge && onTop && falling) return;
    
    if(right || left)
    {
        windForce = 350 * objA->body.damping;
    }

    if(up)
    {
        objA->body.force.y -= windForce;
    }
    else if(down)
    {
        objA->body.force.y += windForce;
    }
    else if(right)
    {
        objA->body.force.x += windForce;
    }
    else if(left)
    {
        objA->body.force.x -= windForce;
    }
}

inline void ApplyWaterPhysics(GameObject* objA, bool gravityUp)
{
    float force = 700 * objA->body.damping;

    if(!gravityUp) objA->body.force.y -= force;
    else objA->body.force.y += force;
}