#include "enemy.h"
#include <iostream>
#include "physics.h"

void Enemy::UpdateRender(float dt)
{
    if(!enemyRenderData) return;

    animationTimer += dt * enemyRenderData->animationSpeed;

    int startFrame = 0;
    int endFrame = enemyRenderData->animationFrames.size();

    enemyRenderData->offset = enemyRenderData->ogOffset;

    switch (type)
    {

    case EnemyType::SWEEPER_A:
    {
        if(std::abs(gameObj.body.velocity.x) > 1.0f)
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
    break;

    case EnemyType::YUUKA:
    {
        bool& isStomping = genericCondition;

        if(!isGrounded)
        {
            if(shooting)
            {
                startFrame = 14;
                endFrame = 14;
            }
            else
            {
                startFrame = 6;
                endFrame = 6;
            }
            
            //moving upwards
            if(
                gameObj.flipData.flipY && gameObj.body.velocity.y > 25.0f ||
                !gameObj.flipData.flipY && gameObj.body.velocity.y < 25.0f
            )
            {
                if(shooting)
                {
                    startFrame = 12;
                    endFrame = 12;
                }
                else
                {
                    startFrame = 5;
                    endFrame = 5;
                }
            }
        }
        else if(isStomping)
        {
            startFrame = 7;
            endFrame = 7;
        }
        else
        {
            if(std::abs(gameObj.body.velocity.x) > 1.0f)
            {
                if(shooting)
                {
                    startFrame = 9;
                    endFrame = 12;
                }
                else
                {
                    startFrame = 1;
                    endFrame = 4;
                }
            }
            else
            {
                if(shooting)
                {
                    startFrame = 8;
                    endFrame = 8;
                }
                else
                {
                    startFrame = 0;
                    endFrame = 0;
                }
            }
        }
    }
    break;
            
    default:
    {
        startFrame = 0;
        endFrame = 0;
    }
    break;
    }

    if(characterCurrentFrame > endFrame || characterCurrentFrame < startFrame)
    {
        characterCurrentFrame = startFrame;

        animationTimer = 0.0f;
    }

    if(animationTimer >= 1.0f)
    {
        animationTimer = 0.0f;

        characterCurrentFrame++;

        if(characterCurrentFrame > endFrame) characterCurrentFrame = startFrame;
    }

    if(characterCurrentFrame < 0) characterCurrentFrame = 0;
}

void Enemy::AmasDroneBehavior(float dt, Player &player)
{
    float distanceToPlayerX = gameObj.transform.position.x - player.gameObj.transform.position.x;
    float distanceToPlayerY = gameObj.transform.position.y - player.gameObj.transform.position.y;

    float hoverSpeed = 40;
    float diveSpeed = 70;

    float triggerRange = TILE_SIZE * 4;

    float hoverHeight = TILE_SIZE * 3;

    bool& isDiving = genericCondition;
    
    if(!isDiving)
    {
        float minDist = 1;

        if(std::abs(distanceToPlayerX) > minDist)
        {
            if(distanceToPlayerX > 0) gameObj.body.velocity.x = -hoverSpeed;
            else gameObj.body.velocity.x = hoverSpeed;
        }
        else
        {
            gameObj.body.velocity.x = 0;
        }

        float currentHoverHeight = hoverHeight;

        if(gameObj.transform.position.y > player.gameObj.transform.position.y)
        {
            currentHoverHeight = -hoverHeight;
        }

        float targetY = (player.gameObj.transform.position.y - currentHoverHeight);

        float distanceToTargetY = targetY - gameObj.transform.position.y;

        if(std::abs(distanceToTargetY) > minDist * 5)
        {
            if(distanceToTargetY > 0) gameObj.body.velocity.y = hoverSpeed;
            else gameObj.body.velocity.y = -hoverSpeed;
        }
        else
        {
            gameObj.body.velocity.y = 0;
        }

        if(std::abs(distanceToPlayerX) < triggerRange && std::abs(distanceToPlayerY) < triggerRange)
        {
            isDiving = true;

            float distanceRatio = std::abs(distanceToPlayerX) / triggerRange;

            float totalDiveXSpeed = hoverSpeed * 1.5f + (hoverSpeed * distanceRatio);

            gameObj.body.velocity.x = (distanceToPlayerX > 0) ? -totalDiveXSpeed : totalDiveXSpeed;

            gameObj.body.velocity.y = (distanceToPlayerY > 0) ? -totalDiveXSpeed : totalDiveXSpeed;
        }
    }
    else
    {
        bool passedPlayer = false;

        if(gameObj.body.velocity.y > 0)
        {
            if(gameObj.transform.position.y > player.gameObj.transform.position.y + triggerRange)
                passedPlayer = true;
        }
        else
        {
            if(gameObj.transform.position.y < player.gameObj.transform.position.y - triggerRange)
                passedPlayer = true;
        }

        if(passedPlayer ||std::abs(distanceToPlayerX) > triggerRange)
        {
            isDiving = false;
        }
    }
}

void Enemy::SweeperABehavior(float dt, Player &player)
{
    lookAtPlayer = true;

    float distanceToPlayerX = gameObj.transform.position.x - player.gameObj.transform.position.x;

    float minDist = 1;

    float moveSpeed = 50;

    if(std::abs(distanceToPlayerX) > minDist)
    {
        if(distanceToPlayerX > 0) gameObj.body.velocity.x = -moveSpeed;
        else gameObj.body.velocity.x = moveSpeed;
    }
    else
    {
        gameObj.body.velocity.x = 0;
    }
}

//re visit later
void Enemy::YuukaBehavior(float dt, Player& player)
{
    float moveSpeed = 50 * moveSpeedSign;

    bool& isStomping = genericCondition;

    switch (currentAttack)
    {

    case Attacks::NOTHING: break;

    case Attacks::STOMP:
    {
        gameObj.body.damping = 0.0f;

        gravity = ogGravity * 0.4f;

        moveSpeedSign = 1;

        maxTime = 0.5f;

        if(isGrounded && timer >= maxTime) isJumping = true;

        if((!isGrounded && hitCeiling)) isJumping = false;

        if(isGrounded)
        {
            if(justLanded && stunState == StunState::NOT_STUNNED && !isJumping)
            {
                if(player.isGrounded)
                {
                    player.ApplyStun(maxTime * aiFrameskip);

                    stunState = StunState::STUNNED;
                }
                else
                {
                    stunState = StunState::DODGED;
                }
            }

            gameObj.body.velocity = {0,0};

            if(timer <= maxTime)
            {
                timer += dt;
            }
        }
        else
        {
            timer = 0.0f;

            stunState = StunState::NOT_STUNNED;
        }
       
        if(isJumping)
        {
            isGrounded = false;

            float jump = -150;

            gameObj.body.velocity.y = jump;

            float distanceToPlayerX = gameObj.transform.position.x - player.gameObj.transform.position.x;

            float airTime = (2 * jump) / gravity;

            float requiredVelocity = distanceToPlayerX / airTime;

            gameObj.body.velocity.x = requiredVelocity * 0.48f;

            isJumping = false;
        }

        isStomping = isGrounded;

        if(stateTimer >= 2.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    case Attacks::STOMP_N_SHOT:
    {
        lookAtPlayer = true;

        gravity = ogGravity * 0.4f;

        bulletData.speed = ogBulletData.speed * 3.0f;

        bulletData.pelletCount = 4;

        bulletData.spread = 6.0f;

        gameObj.body.damping = 0.0f;

        moveSpeedSign = 1;

        maxTime = 0.5f;

        if(isGrounded && timer >= maxTime) isJumping = true;

        if((!isGrounded && hitCeiling)) isJumping = false;

        if(isGrounded)
        {
            if(justLanded && stunState == StunState::NOT_STUNNED && !isJumping)
            {
                if(player.isGrounded)
                {
                    player.ApplyStun(maxTime * aiFrameskip);

                    stunState = StunState::STUNNED;
                }
                else
                {
                    stunState = StunState::DODGED;
                }
            }

            gameObj.body.velocity = {0,0};

            if(timer <= maxTime)
            {
                timer += dt;
            }
        }
        else
        {
            timer = 0.0f;

            stunState = StunState::NOT_STUNNED;
        }
       
        if(isJumping)
        {
            isGrounded = false;

            float jump = -140;

            gameObj.body.velocity.y = jump;

            float rawVelocityX = 50;

            float velocityX = (float)GetRandomValue(-rawVelocityX, rawVelocityX);

            gameObj.body.velocity.x = velocityX;

            isJumping = false;
        }

        isStomping = isGrounded;

        int shoots = GetRandomValue(1, 100);

        shooting = ((shoots <= 80) && isGrounded && !isJumping);

        if(stateTimer >= 3.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    case Attacks::RUN_N_SHOOT:
    {
        gameObj.body.velocity.x = moveSpeed;

        bulletData.fireRate = 1.2f;

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

        if(stateTimer >= 4.0f)
        {
            stateTimer = 0.0f;
        }
    }
    break;

    default:
    {
        stateTimer = 0.0f;
    }
    break;
    }
}

void Enemy::UpdateAI(float dt, Player& player)
{
    if(!lookAtPlayer) gameObj.body.velocity.x >= 0 ? gameObj.flipData.flipX = false : gameObj.flipData.flipX = true;
    else
    {
        float distanceToPlayerX = gameObj.transform.position.x - player.gameObj.transform.position.x;

        distanceToPlayerX >= 0 ? gameObj.flipData.flipX = true : gameObj.flipData.flipX = false;
    }
    
    if(stateTimer <= 0.0f)
    {
        lastAttack = currentAttack;

        int roll = GetRandomValue(1,100);

        if(roll <= 50) moveSpeedSign = -1;
        else moveSpeedSign = 1;

        if (roll <= 50) currentAttack = Attacks::STOMP;
        else if(roll <= 80) currentAttack = Attacks::STOMP_N_SHOT;
        else currentAttack = Attacks::RUN_N_SHOOT;

        if(lastAttack != currentAttack)
        {
            lookAtPlayer = false;

            timer = maxTime;

            gravity = ogGravity;

            gameObj.body.velocity = {0,0};
            
            gameObj.body.altVelocity = {0,0};

            if(canFly) gameObj.body.hasGravity = false;
            else gameObj.body.hasGravity = true;

            shooting = false;

            genericCondition = false;

            gameObj.body.damping = ogDamping;

            bulletData = ogBulletData;
        }
    }

    if(randomAttack) stateTimer += dt;

    switch (type)
    {

    case EnemyType::AMAS_DRONE: AmasDroneBehavior(dt, player); break;

    case EnemyType::SWEEPER_A: SweeperABehavior(dt, player); break;

    case EnemyType::YUUKA: YuukaBehavior(dt, player); break;

    default: break;
    }
}

void Enemy::Shoot(float dt)
{
    if(!bulletpool) return;

    ShootBullet(
        dt, gameObj,
        bulletData, GetTextureBulletSpawnPos(gameObj, weaponRenderData),
        bulletpool.get(), shooting
    );
}

void Enemy::InitEnemy(
    const Vector2& spawnPos,
    const SpriteFlipData& data,
    int paletteIndex,
    float gravity,
    int frameskip
)
{
    gameObj.body = {};

    gameObj.UpdateHitboxes();

    spawnFlipData = data;

    gameObj.flipData = spawnFlipData;

    this->gravity = gravity;

    ogGravity = this->gravity;

    bulletData = {};

    bulletData.explodes = false;

    bulletData.mainColor = GOLD;
    bulletData.backColor = BLACK;

    bulletData.explosionRadius = 15.0f;

    Hitbox mainHitbox = {
        {0,0},
        {6,14}
    };

    Vector2 weaponOffset = {0,0};

    switch (type)
    {
    case EnemyType::DUMMY:
    {
        testColor = ENEMY_DUMMY;

        canFly = false;
    }
    break;

    case EnemyType::AMAS_DRONE:
    {
        mainHitbox.aabb.width = 12;
        mainHitbox.aabb.height = 6;

        gameObj.body.damping = 0;

        canFly = true;
    }
    break;

    case EnemyType::SWEEPER_A:
    {
        mainHitbox.aabb.width = 7;
        mainHitbox.aabb.height = 11;
    }
    break;

    case EnemyType::YUUKA:
    {
        testColor = ENEMY_YUUKA;

        canFly = false;

        randomAttack = true;

        bulletData.angle = 0;
        bulletData.fireTimer = 0.0f;
        bulletData.speed = 36;
        
        bulletData.fireRate = 1.0f;

        bulletData.spread = 1.0f;

        bulletData.radius = 1.5f;

        bulletData.lifeTime = 4.0f;
    }
    break;
    
    default:
        break;
    }

    gameObj.hitboxes.push_back(mainHitbox);

    float verticalOffset = 7.0f;

    //jump detector
    gameObj.AddSubHitbox({0,verticalOffset}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    //ceiling detector
    gameObj.AddSubHitbox({0,-verticalOffset}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    spawnPosition = spawnPos;

    spawnPosition.y -= gameObj.GetMainAABB().height * 0.25f;
    
    gameObj.transform.position = spawnPosition;

    bulletpool = std::make_unique<BulletPool>(30, bulletData);

    enemyRenderData = GetEnemyActiveRenderData(type, characterVariantIndex);

    characterPaletteIndex = paletteIndex;

    aiFrameskip = frameskip;

    ogDamping = gameObj.body.damping;

    ogBulletData = bulletData;

    if(canFly) gameObj.body.hasGravity = false;
    else gameObj.body.hasGravity = true;

    if(!randomAttack) stateTimer = 1000;
}

void Enemy::Update(float dt, int iterations)
{
    FlipHitboxY(gameObj.hitboxes[1], gameObj.flipData.flipY, false);

    FlipHitboxY(gameObj.hitboxes[2], gameObj.flipData.flipY, true);
}