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

void SolveCollisions(GameObject& objA, const Rectangle& recB, const Vector2& vel ,bool isX, bool gravityUp, bool isTrampoline, bool isPlatform);

void SolveCollisions_Platform(GameObject& objA, const Rectangle& recB, bool isX);


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

inline void SolveCollisionsOneWayLeftRight(GameObject& objA, const Rectangle& recB, Vector2 vel ,bool isRight)
{
    bool isLeft = !isRight;

    float offset = 6.0f;

    if(isLeft) if(IsLeft(objA.GetMainAABB(), recB, offset) || objA.body.velocity.x <= 0.0f) return;

    if(isRight) if(IsRight(objA.GetMainAABB(), recB, offset) || objA.body.velocity.x >= 0.0f) return;

    SolveCollisions(
        objA,
        recB, 
        vel,
        true, false, false, false
    );
}

inline void SolveCollisionsOneWayUpDown(GameObject& objA, const Rectangle& recB, Vector2 vel, bool isUp ,bool gravityUp, bool isPlatform)
{
    bool isDown = !isUp;

    float offset = 5.0f;

    if(!isPlatform)
    {
        if(isUp) if(!IsAbove(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y <= 0.0f) return;

        if(isDown) if(!IsBelow(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y >= 0.0f) return;
    }
    else
    {
        if(!gravityUp)
        {
            if(isUp) if(!IsAbove(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y <= 0.0f) return;

            if(isDown) if(!IsBelow(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y >= 0.0f) return;
        }
        else
        {
            if(isDown) if(!IsAbove(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y <= 0.0f) return;

            if(isUp) if(!IsBelow(objA.GetMainAABB(), recB, offset) || objA.body.velocity.y >= 0.0f) return;
        }
    }

    SolveCollisions(
        objA,
        recB,
        vel,
        false, gravityUp, false, isPlatform
    );
}

inline void ApplyWind(
    GameObject& objA,
    const Rectangle& aabb,
    Direction direction, 
    bool isEdgeUp, bool isEdgeDown,
    bool gravityUp
)
{ 
    bool up = direction == Direction::UP;
    
    bool down = direction == Direction::DOWN;

    bool left = direction == Direction::LEFT;

    bool right = direction == Direction::RIGHT;

    float windForce = 520 * objA.body.damping;

    bool isEdge = (!gravityUp && isEdgeUp) || (gravityUp && isEdgeDown);

    float offset = aabb.height * 0.8f;

    bool above = IsAbove(objA.GetMainAABB(), aabb, offset);

    bool below = IsBelow(objA.GetMainAABB(), aabb, offset);

    bool onTop = !gravityUp ? above : below;

    bool falling = !gravityUp ? objA.body.velocity.y > 0 : objA.body.velocity.y < 0;

    if(isEdge && onTop && falling) return;
    
    if(right || left)
    {
        windForce *= 0.7f;
    }

    if(up)
    {
        objA.body.force.y -= windForce;
    }
    else if(down)
    {
        objA.body.force.y += windForce;
    }
    else if(right)
    {
        objA.body.force.x += windForce;
    }
    else if(left)
    {
        objA.body.force.x -= windForce;
    }
}

inline void ApplyWaterPhysics(GameObject* objA, bool gravityUp)
{
    float force = 175 * objA->body.damping; //175

    if(!gravityUp) objA->body.force.y -= force;
    else objA->body.force.y += force;
}