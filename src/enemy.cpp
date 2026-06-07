#include "enemy.h"
#include <iostream>
#include "physics.h"

void Enemy::YuukaBehaivour(float dt, const Vector2& playerPos, Vector2* playerVel, const bool isPlayerGrounded, bool* canPlayerMove)
{
    float moveSpeed = 200 * moveSpeedSign;

    //std::cout<<"state timer: "<<stateTimer<<"\n";

    if(stateTimer <= 0.0f)
    {
        gameObj.body.velocity = {0,0};
        gameObj.body.altVelocity = {0,0};

        gameObj.body.hasGravity = true;

        int roll = GetRandomValue(0,100);

        if(roll <= 50) moveSpeedSign = -1;
        else moveSpeedSign = 1;

        if (roll <= 50) currentAttack = 0;
        else currentAttack = 1;

        counter = 0;
    }

    stateTimer += dt;

    switch (currentAttack)
    {
    // stomp
    case 0:
    {
        moveSpeedSign = 1;

        maxTime = 0.5f;

        float distToPlayerSqr = Vector2LengthSqr(playerPos - gameObj.transform.position);

        const float NUM_OF_TILES = 20.0f;

        float minDist = NUM_OF_TILES * GRID_SIZE * NUM_OF_TILES * GRID_SIZE;

        if(!isGrounded && gameObj.body.velocity.y <= 0.1f) isJumping = false;

        if(isGrounded && distToPlayerSqr <= minDist)
        {
            if(timer <= 0.0f) isJumping = true;
        }

        if(!isGrounded)
        {
            timer = maxTime;
            counter = 0;
            *canPlayerMove = true;
        }
        else
        {
            if(counter == 0)
            {
                if(isPlayerGrounded) counter = 2;
                else counter = 1;
            }

            if(timer > 0.0f)
            {
                timer -= dt;

                gameObj.body.velocity.x = 0;

                if(counter == 2) 
                {
                    *canPlayerMove = false;
                    playerVel->x = 0;
                }
            }
        }
       
        if(isJumping)
        {
            float xPosDifference = playerPos.x - gameObj.transform.position.x;

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

        if(stateTimer >= 2.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    case 1:
    {
        gameObj.body.velocity.x = moveSpeed;
        
        if(isTouchingWall)
        {
            if(counter == 0)
            {
                float offset = 4.0f;

                if(moveSpeedSign > 0)
                {
                    gameObj.transform.position.x -= offset;
                }
                else if(moveSpeedSign < 0)
                {
                    gameObj.transform.position.x += offset;
                }

                moveSpeedSign *= -1;

                counter = 1;
            }
        }
        else
        {
            counter = 0;
        }

        if(stateTimer >= 6.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    default:break;
    }
}

void Enemy::UpdateAI(float dt, const Vector2& playerPos, Vector2* playerVel, const bool isPlayerGrounded, bool* canPlayerMove)
{
    switch (type)
    {

    case EnemyType::YUUKA: YuukaBehaivour(dt, playerPos, playerVel, isPlayerGrounded, canPlayerMove); break;

    default: break;
    }
}

void Enemy::Update(float dt, int iterations)
{
    gameObj.body.UpdateVelocity(dt, iterations, gravity);
}