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

        gravity = ogGravity;

        shooting = false;

        int roll = GetRandomValue(0,100);

        if(roll <= 50) moveSpeedSign = -1;
        else moveSpeedSign = 1;

        if (roll <= 80) currentAttack = 0;
        else currentAttack = 1;

        counter = 0;
    }

    stateTimer += dt;

    switch (currentAttack)
    {
    // stomp
    case 0:
    {
        gravity = ogGravity * 2.0f;

        moveSpeedSign = 1;

        maxTime = 0.5f;

        float distToPlayerSqr = Vector2LengthSqr(playerPos - gameObj.transform.position);

        const float NUM_OF_TILES = 20.0f;

        float minDist = NUM_OF_TILES * GRID_SIZE * NUM_OF_TILES * GRID_SIZE;

        if(!isGrounded && hitCeiling) isJumping = false;

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
            isGrounded = false;

            float xPosDifference = playerPos.x - gameObj.transform.position.x;

            float jumpVel = -10000;

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

            isJumping = false;
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

        if(stateTimer >= 0.5f) shooting = true;
        
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

                gameObj.data.flipX = !gameObj.data.flipX;

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

void Enemy::Shoot(float dt)
{
    ShootBullet(
        dt, gameObj,
        &bulletData, GetBulletSpawnPos(),
        bulletpool.get(), shooting
    );
}

void Enemy::InitEnemy(
    const Vector2& spawnPos,
    const EntityData& data,
    float gravity
)
{
    spawnPosition = spawnPos;
    
    gameObj.transform.position = spawnPosition;

    gameObj.hasBody = true;
    gameObj.body = {};

    gameObj.UpdateHitboxes();

    spawnData = data;

    gameObj.data = spawnData;

    this->gravity = gravity;

    ogGravity = this->gravity;

    bulletData = {};

    bulletData.explodes = false;

    bulletData.mainColor = GOLD;
    bulletData.backColor = BLACK;

    bulletData.explosionRadius = 60.0f;

    Hitbox mainHitbox = {
        {0,0},
        {20,38}
    };

    Hitbox jumpDetector = {
        {0,0},
        {mainHitbox.aabb.width * 0.9f, mainHitbox.aabb.height * 0.5f}
    };

    switch (type)
    {
    case EnemyType::DUMMY:
    {
        testColor = ENEMY_DUMMY;

        gameObj.body.hasGravity = true;
    }
    break;

    case EnemyType::YUUKA:
    {
        testColor = ENEMY_YUUKA;

        gameObj.body.hasGravity = true;

        bulletData.angle = 0;
        bulletData.fireTimer = 0.0f;
        bulletData.speed = 150;
        
        bulletData.fireRate = 1.0f;

        bulletData.spread = 1.0f;

        bulletData.radius = 6.0f;

        bulletData.lifeTime = 2.0f;
    }
    break;
    
    default:
        break;
    }

    gameObj.hitboxes.push_back(mainHitbox);

    gameObj.hitboxes.push_back(jumpDetector);

    bulletpool = std::make_unique<BulletPool>(30, bulletData);
}

void Enemy::UpdateAI(float dt, const Vector2 &playerPos, Vector2 *playerVel, const bool isPlayerGrounded, bool *canPlayerMove)
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