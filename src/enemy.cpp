#include "enemy.h"
#include <iostream>

void Enemy::YuukaBehaivour(float dt, int iterations, const Vector2& playerPos)
{
    float moveForce = 800 * gameObj.body.damping;

    float subDt = dt / iterations;

    if(stateTimer <= 0.0f)
    {
        int roll = GetRandomValue(0,100);

        if (roll <= 100) currentAttack = 0;

        counter = 0;
    }

    stateTimer += dt;

    switch (currentAttack)
    {
    case 0:
    {
        float distToPlayerSqr = Vector2LengthSqr(playerPos - gameObj.transform.position);

        float minDist = 1.5f * GRID_SIZE * 1.5f * GRID_SIZE;

        if(isJumping)
        {
            gameObj.body.hasGravity = false;
        }
        else
        {
            gameObj.body.hasGravity = true;
        }

        if(!isGrounded && std::abs(gameObj.body.velocity.y <= 0.1f)) isJumping = false;

        if(!isJumping)
        {
            jumpTime = maxJumpTime;

            if(gameObj.transform.position.x > playerPos.x)
            {
                gameObj.body.force.x -= moveForce;
            }
            else
            {
                gameObj.body.force.x += moveForce;
            }

            if(distToPlayerSqr <= minDist && distToPlayerSqr >= -minDist)
            {
                if(isGrounded)isJumping = true;
            }
        }
        else
        {
            jumpTime -= subDt;

            if(jumpTime <= 0.0f) jumpTime = 0.0f;
        }
       
        if(isJumping)
        {
            float jumpVel = -10000 * 70;

            isGrounded = false;

            gameObj.body.velocity.y += jumpVel * subDt;

            if(jumpTime <= 0.0f) isJumping = false;
        }

        if(!isJumping && !isGrounded)
        {
            gameObj.body.force.y += moveForce * 10;
        }

        //WIP
        if(counter >= 3 && isGrounded)
        {
            stateTimer = 0.0f;
        }
    }
    break;
    default:
        break;
    }
}

void Enemy::UpdateAI(float dt, int iterations, const Vector2 &playerPos)
{
    switch (type)
    {

    case EnemyType::YUUKA: YuukaBehaivour(dt, iterations, playerPos); break;

    default: break;
    }
}

void Enemy::Update(float dt, int iterations)
{
    gameObj.body.UpdateVelocity(dt, iterations, gravity);
}