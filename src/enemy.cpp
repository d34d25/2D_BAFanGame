#include "enemy.h"
#include <iostream>
#include "physics.h"

void Enemy::UpdateRender(float dt)
{
    if(enemyRenderData)
    {
        animationTimer += dt * enemyRenderData->animationSpeed;

        if(animationTimer >= 1.0f)
        {
            animationTimer = 0.0f;

            int startFrame = 0;
            int endFrame = enemyRenderData->animationFrames.size();

            enemyRenderData->offset = enemyRenderData->ogOffset;

            switch (type)
            {
            case EnemyType::YUUKA:
            {
                if(!isGrounded)
                {
                    startFrame = 4;
                    endFrame = 4;
                }
                else if(isStomping)
                {
                    startFrame = 5;
                    endFrame = 5;
                }
                else
                {
                    if(std::abs(gameObj.body.velocity.x) > 50.0f)
                    {
                        startFrame = 1;
                        endFrame = 3;
                    }
                    else
                    {
                        startFrame = 0;
                        endFrame = 0;
                    }
                }
            }
            break;
            
            default:
                break;
            }

            currentFrame++;

            if(currentFrame > endFrame || currentFrame < startFrame) currentFrame = startFrame;

            if(currentFrame < 0) currentFrame = 0;
        }
    }
}

void Enemy::YuukaBehaivour(float dt, int framskip, Player& player)
{
    float moveSpeed = 200 * moveSpeedSign;

    //std::cout<<"state timer: "<<stateTimer<<"\n";

    gameObj.body.velocity.x >= 0 ? gameObj.data.flipX = false : gameObj.data.flipX = true;

    if(stateTimer <= 0.0f)
    {
        gameObj.body.velocity = {0,0};
        gameObj.body.altVelocity = {0,0};

        gameObj.body.hasGravity = true;

        gravity = ogGravity;

        shooting = false;

        isStomping = false;

        timer = 0.0f;

        int roll = GetRandomValue(0,100);

        if(roll <= 50) moveSpeedSign = -1;
        else moveSpeedSign = 1;

        if (roll <= 70) currentAttack = Attacks::STOMP;
        else currentAttack = Attacks::RUN_N_SHOOT;
    }

    stateTimer += dt;

    switch (currentAttack)
    {

    case Attacks::NOTHING: break;

    case Attacks::STOMP:
    {
        gravity = ogGravity * 2.0f;

        moveSpeedSign = 1;

        maxTime = 0.5f;

        if(!isGrounded && hitCeiling) isJumping = false;
        else if(isGrounded && timer >= maxTime) isJumping = true;

        if(!isGrounded)
        {
            timer = 0.0f;

            stunState = StunState::NOT_STUNNED;
        }
        else
        {
            if(stunState == StunState::NOT_STUNNED && !isJumping)
            {
                if(player.isGrounded)
                {
                    player.ApplyStun(maxTime * framskip);

                    stunState = StunState::STUNNED;
                }
                else
                {
                    stunState = StunState::DODGED;
                }
            }

            if(timer <= maxTime)
            {
                timer += dt;
            }
        }
       
        if(isJumping)
        {
            isGrounded = false;

            float xPosDifference = player.gameObj.transform.position.x - gameObj.transform.position.x;

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

        isStomping = isGrounded;

        if(stateTimer >= 2.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    case Attacks::RUN_N_SHOOT:
    {
        gameObj.body.velocity.x = moveSpeed;

        if(stateTimer >= 0.5f) shooting = true;
        
        if(isTouchingWall)
        {
            if(!alreadyFlipped)
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

                alreadyFlipped = true;
            }
        }
        else
        {
            alreadyFlipped = false;
        }

        if(stateTimer >= 6.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    case Attacks::WIP:
    {
        stateTimer = 0.0f;
    }
    break;

    default:
    {
        stateTimer = 0.0f;
    }
    break;
    }
}

void Enemy::Shoot(float dt)
{
    ShootBullet(
        dt, gameObj,
        &bulletData, GetTextureBulletSpawnPos(gameObj, weaponRenderData),
        bulletpool.get(), shooting
    );
}

void Enemy::InitEnemy(
    const Vector2& spawnPos,
    const EntityData& data,
    float gravity
)
{
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
        {25,56}
    };

    Vector2 weaponOffset = {0,0};

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

    gameObj.AddSubHitbox({0,23}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    gameObj.AddSubHitbox({0,-23}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    spawnPosition = spawnPos;

    spawnPosition.y -= gameObj.GetMainAABB().height * 0.25f;
    
    gameObj.transform.position = spawnPosition;

    bulletpool = std::make_unique<BulletPool>(30, bulletData);

    enemyRenderData = GetEnemyActiveRenderData(type, variantIndex);

    weaponRenderData = GetEnemyWeaponRenderData(type);
}

void Enemy::UpdateAI(float dt, int framskip, Player& player)
{
    switch (type)
    {

    case EnemyType::YUUKA: YuukaBehaivour(dt, framskip, player); break;

    default: break;
    }
}

void Enemy::Update(float dt, int iterations)
{
    FlipHitboxY(gameObj.hitboxes[1], gameObj.data.flipY, false);

    FlipHitboxY(gameObj.hitboxes[2], gameObj.data.flipY, true);

    gameObj.body.UpdateVelocity(dt, iterations, gravity);
}