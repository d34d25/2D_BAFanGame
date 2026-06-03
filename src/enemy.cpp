#include "enemy.h"
#include <iostream>

void Enemy::YuukaBehaivour(float dt, const Vector2 &playerPos)
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

        const float NUM_OF_TILES = 6.0f;

        float minDist = NUM_OF_TILES * GRID_SIZE * NUM_OF_TILES * GRID_SIZE;

        float xPosDifference = playerPos.x - gameObj.transform.position.x;

        if(!isGrounded && std::abs(gameObj.body.velocity.y <= 0.1f)) isJumping = false;

        if(isGrounded)
        {
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

            if(distToPlayerSqr <= minDist)
            {
                if(timer <= 0.0f) isJumping = true;
            }
        }

        if(!isGrounded)
        {
            timer = maxTime;
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
            float jumpVel = -5000;

            isGrounded = false;

            gameObj.body.velocity.y = jumpVel;

            float horizontalBoostFactor = std::abs(xPosDifference) * 0.02f;

            if(xPosDifference > 0)
            {
                gameObj.body.velocity.x = moveSpeed * horizontalBoostFactor;
            }
            else
            {
                gameObj.body.velocity.x = -moveSpeed * horizontalBoostFactor;
            }
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