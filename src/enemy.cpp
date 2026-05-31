#include "enemy.h"
#include <iostream>

void Enemy::YuukaBehaivour(float dt, const Vector2& playerPos)
{
    float moveSpeed = 200;

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
        maxTime = 0.5f;

        float distToPlayerSqr = Vector2LengthSqr(playerPos - gameObj.transform.position);

        const float NUM_OF_TILES = 2.0f;

        float minDist = NUM_OF_TILES * GRID_SIZE * NUM_OF_TILES * GRID_SIZE;

        if(isJumping)
        {
            gameObj.body.hasGravity = false;
        }
        else
        {
            gameObj.body.hasGravity = true;
        }

        if(!isGrounded && std::abs(gameObj.body.velocity.y <= 0.1f)) isJumping = false;

        if(gameObj.body.velocity.y <= 0)
        {
            jumpTime = maxJumpTime;

            float xPosDifference = playerPos.x - gameObj.transform.position.x;

            float offset = 7.0f;

            if(std::abs(xPosDifference) > offset)
            {
                if(xPosDifference > 0)
                {
                    gameObj.body.velocity.x = moveSpeed;
                }
                else
                {
                    gameObj.body.velocity.x = -moveSpeed;
                }
            }
            else
            {
                gameObj.body.velocity.x = 0;
            }

            if(distToPlayerSqr <= minDist && distToPlayerSqr >= -minDist)
            {
                if(isGrounded && timer <= 0.0f) isJumping = true;
            }
        }

        if(!isGrounded)
        {
            timer = maxTime;

            jumpTime -= dt;

            if(jumpTime <= 0.0f) jumpTime = 0.0f;
        }
        else
        {
            if(timer > 0.0f)
            {
                timer -= dt;
            }
        }
       
        if(isJumping)
        {
            float jumpVel = -10000 * 70;

            isGrounded = false;

            gameObj.body.velocity.y += jumpVel * dt;

            if(jumpTime <= 0.0f) isJumping = false;
        }

        if(!isJumping && !isGrounded && gameObj.body.velocity.y > 0)
        {
            gameObj.body.force.y += moveSpeed * 10;
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

void Enemy::UpdateAI(float dt, const Vector2 &playerPos)
{
    switch (type)
    {

    case EnemyType::YUUKA: YuukaBehaivour(dt, playerPos); break;

    default: break;
    }
}

void Enemy::Update(float dt, int iterations)
{
    gameObj.body.UpdateVelocity(dt, iterations, gravity);
}