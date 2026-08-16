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

    case EnemyType::BOMBER_DRONE:
    {
        if(shooting)
        {
            startFrame = 1;
            endFrame = 1;
        }
        else
        {
            startFrame = 0;
            endFrame = 0;
        }
    }
    break;

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

    case EnemyType::SWEEPER_B:
    {
        if(!shooting)
        {
            startFrame = 1;
            endFrame = 1;
        }
        else
        {
            startFrame = 0;
            endFrame = 0;
        }
    }
    break;

    case EnemyType::HELMET_GANG:
    {
        //bool& blocking = genericCondition;

        if(!shooting)
        {
            if(!isGrounded)
            {
                startFrame = 3;
                endFrame = 3;
            }
            else
            {
                startFrame = 1;
                endFrame = 1;
            }
        }
        else
        {
            if(!isGrounded)
            {
                startFrame = 4;
                endFrame = 4;
            }
            else
            {
                startFrame = 2;
                endFrame = 2;
            }
        }
    }
    break;

    case EnemyType::AMAS_HEAVY:
    {
        if(shooting)
        {
            startFrame = 1;
            endFrame = 1;
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

        if(playingIntro)
        {
            startFrame = 0;
            endFrame = 0;
        }
        else if(!isGrounded)
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

/*
enemies to add:

stationary turret (8 (done) and 5 direction)

a simple jumper enemy (yuuka's stomp pattern but simpler)

and 1 boss more (at least)

number of stages, around 4 or 5 (at least)
*/

//vector difference calculation is always target - source

void Enemy::AmasDroneBehavior(float dt, Player &player)
{
    float distanceToPlayerX = player.gameObj.transform.position.x - gameObj.transform.position.x;
    float distanceToPlayerY = player.gameObj.transform.position.y - gameObj.transform.position.y;

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
            if(distanceToPlayerX <= 0) gameObj.body.velocity.x = -hoverSpeed;
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
            if(distanceToTargetY <= 0) gameObj.body.velocity.y = -hoverSpeed;
            else gameObj.body.velocity.y = hoverSpeed;
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

            gameObj.body.velocity.x = (distanceToPlayerX <= 0) ? -totalDiveXSpeed : totalDiveXSpeed;

            gameObj.body.velocity.y = (distanceToPlayerY <= 0) ? -totalDiveXSpeed : totalDiveXSpeed;
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

void Enemy::AmasDroneBBehavior(float dt, Player &player)
{
    float dist = TILES_PER_ROOM_WIDHT * TILE_SIZE;

    shooting = Vector2DistanceSqr(player.gameObj.transform.position, gameObj.transform.position) <= (dist * dist);
}

void Enemy::BomberDroneBehavior(float dt, Player &player)
{
    float distanceToPlayerX = player.gameObj.transform.position.x - gameObj.transform.position.x;

    float hoverSpeed = 40;

    float minDist = 1;

    float hoverHeight = TILE_SIZE * 4;

    if(std::abs(distanceToPlayerX) > minDist)
    {
        if(distanceToPlayerX <= 0) gameObj.body.velocity.x = -hoverSpeed;
        else gameObj.body.velocity.x = hoverSpeed;
    }
    else
    {
        gameObj.body.velocity.x = 0;
    }

    shooting = std::abs(distanceToPlayerX) <= TILE_SIZE;
    
    float currentHoverHeight = hoverHeight;

    if((gravity < 0) && (gameObj.transform.position.y > player.gameObj.transform.position.y))
    {
        currentHoverHeight = -hoverHeight;
    }

    float targetY = (player.gameObj.transform.position.y - currentHoverHeight);

    float distanceToTargetY = targetY - gameObj.transform.position.y;

    if(std::abs(distanceToTargetY) > minDist * 5)
    {
        if(distanceToTargetY <= 0) gameObj.body.velocity.y = -hoverSpeed;
        else gameObj.body.velocity.y = hoverSpeed;
    }
    else
    {
        gameObj.body.velocity.y = 0;
    }
}

void Enemy::SweeperABehavior(float dt, Player &player)
{
    float distanceToPlayerX = player.gameObj.transform.position.x - gameObj.transform.position.x;

    float minDist = 1;

    float moveSpeed = 50;

    if(std::abs(distanceToPlayerX) > minDist)
    {
        if(distanceToPlayerX <= 0) gameObj.body.velocity.x = -moveSpeed;
        else gameObj.body.velocity.x = moveSpeed;
    }
    else
    {
        gameObj.body.velocity.x = 0;
    }
}

void Enemy::SweeperBBehavior(float dt, Player &player)
{
    timer += dt;

    if(timer >= maxTime)
    {
        timer = 0.0f;

        shooting = !shooting;

        if(shooting) bulletData.fireTimer = 0;
    }
}

void Enemy::HelmetGangBehavior(float dt, Player &player)
{
    gravity = ogGravity * 0.75f;

    timer += dt;
    
    if(timer >= maxTime)
    {
        timer = 0.0f;

        shooting = !shooting;

        if(shooting) bulletData.fireTimer = 0;
    }

    bool playerInRange = false;

    float dist = TILE_SIZE * 10;

    if(Vector2DistanceSqr(gameObj.transform.position, player.gameObj.transform.position) <= dist * dist)
    {
        int threshold = TILE_SIZE;

        bool isPlayerAbove = (gravity >= 0 && player.gameObj.transform.position.y < 
        gameObj.transform.position.y - threshold);

        if(isPlayerAbove && canJump && isGrounded)
        {
            canJump = false;

            float jumpVel = -400;

            if(gravity < 0) jumpVel = -jumpVel;

            gameObj.body.velocity.y = jumpVel;

            isGrounded = false;
        }
    }

    if(isGrounded) canJump = true;
}

void Enemy::AmasHeavyBehavior(float dt, Player &player)
{
    float dist = TILE_SIZE * 6;

    float playerYDist = player.gameObj.transform.position.y - gameObj.transform.position.y;

    if(std::abs(playerYDist) <= TILE_SIZE * 3)
    {
        if(
            !gameObj.flipData.flipX && gameObj.transform.position.x <= player.gameObj.transform.position.x ||
            gameObj.flipData.flipX && gameObj.transform.position.x > player.gameObj.transform.position.x
        )
        {
            shooting = true;
        }
        else
        {
            shooting = false;
        }
    }
    else
    {
        shooting = false;
    }
}

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

        if(isGrounded && timer >= maxTime) canJump = true;

        if((!isGrounded && hitCeiling)) canJump = false;

        if(isGrounded)
        {
            if(justLanded && stunState == StunState::NOT_STUNNED && !canJump)
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
       
        if(canJump)
        {
            isGrounded = false;

            float jump = -150;

            gameObj.body.velocity.y = jump;

            //distance to target is always target - source
            float distanceToPlayerX = player.gameObj.transform.position.x - gameObj.transform.position.x;

            float airTime = fabsf(2.0f * jump) / gravity;

            float requiredVelocity = distanceToPlayerX / airTime;

            gameObj.body.velocity.x = requiredVelocity;

            canJump = false;
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

        if(isGrounded && timer >= maxTime) canJump = true;

        if(hitCeiling) canJump = false;

        if(isGrounded)
        {
            if(justLanded && stunState == StunState::NOT_STUNNED && !canJump)
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
       
        if(canJump)
        {
            canJump = false;

            float jump = -140;

            gameObj.body.velocity.y = jump;

            float rawVelocityX = 50;

            float velocityX = (float)GetRandomValue(-rawVelocityX, rawVelocityX);

            gameObj.body.velocity.x = velocityX;

            isGrounded = false;
        }

        isStomping = isGrounded;

        int shoots = GetRandomValue(1, 100);

        shooting = ((shoots <= 80) && isGrounded && !canJump);

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

        if(stateTimer >= 2.5f)
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
    if(!IsEnemyStatic(type))
    {
        if(!lookAtPlayer) gameObj.body.velocity.x >= 0 ? gameObj.flipData.flipX = false : gameObj.flipData.flipX = true;
        else player.gameObj.transform.position.x > gameObj.transform.position.x ? gameObj.flipData.flipX = false : gameObj.flipData.flipX = true;
    }

    PlayIntro(dt);

    //std::cout<<"enemy "<< this <<" intro time: "<< introTimer<<"\n";

    if(playingIntro) return;

    if(stateTimer <= 0.0f)
    {
        lastAttack = currentAttack;

        int roll = GetRandomValue(1,100);

        if(roll <= 50) moveSpeedSign = -1;
        else moveSpeedSign = 1;

        switch (type)
        {
        case EnemyType::YUUKA:
        {
            if(roll <= 50) currentAttack = Attacks::STOMP;
            else if(roll <= 80) currentAttack = Attacks::STOMP_N_SHOT;
            else currentAttack = Attacks::RUN_N_SHOOT;

        }
        break;
        
        default: currentAttack = Attacks::NOTHING; break;
        }
       
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

            stunState = StunState::NOT_STUNNED;
        }
    }

    if(randomAttack) stateTimer += dt;

    switch (type)
    {
    case EnemyType::AMAS_DRONE: AmasDroneBehavior(dt, player); break;

    case EnemyType::AMAS_DRONE_B: AmasDroneBBehavior(dt, player); break;

    case EnemyType::BOMBER_DRONE: BomberDroneBehavior(dt, player); break;

    case EnemyType::SWEEPER_A: SweeperABehavior(dt, player); break;

    case EnemyType::SWEEPER_B: SweeperBBehavior(dt, player); break;

    case EnemyType::YUUKA: YuukaBehavior(dt, player); break;

    case EnemyType::HELMET_GANG: HelmetGangBehavior(dt, player); break;

    case EnemyType::AMAS_HEAVY: AmasHeavyBehavior(dt, player); break;

    default: break;
    }
}

void Enemy::InitEnemy(
    const Vector2& spawnPos,
    const SpriteFlipData& data,
    int paletteIndex,
    float gravity,
    int frameskip
)
{
    health = maxHealth;

    gameObj.body = {};

    spawnFlipData = data;

    this->gravity = gravity;

    ogGravity = this->gravity;

    bulletData = {};

    bulletData.explodes = false;

    bulletData.explosionRadius = 15.0f;

    characterPaletteIndex = paletteIndex;

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

        maxHealth = 1;
    }
    break;

    case EnemyType::AMAS_DRONE:
    {
        mainHitbox.aabb.width = 12;
        mainHitbox.aabb.height = 6;

        gameObj.body.damping = 0;

        canFly = true;

        maxHealth = 1;
    }
    break;

    case EnemyType::AMAS_DRONE_B:
    {
        mainHitbox.aabb.width = 12;
        mainHitbox.aabb.height = 6;

        gameObj.body.damping = 0;

        bulletData.fanShaped = true;

        bulletData.fireRate = 1.15f;

        bulletData.inertia = false;

        bulletData.spread = 45;

        bulletData.pelletCount = 8;

        bulletData.speed = 90;

        canFly = true;

        bulletData.radius = 1;

        lookAtPlayer = true;

        maxHealth = 1;
    }
    break;

    case EnemyType::BOMBER_DRONE:
    {
        mainHitbox.aabb.width = 12;
        mainHitbox.aabb.height = 6;

        gameObj.body.damping = 0;

        bulletData.speed = 50;

        bulletData.angle = 90;

        canFly = true;

        bulletData.inertia = false;

        bulletData.fireRate = 0.6f;

        bulletData.explosionRadius = 12;

        bulletData.explodes = true;

        maxHealth = 1;
    }
    break;

    case EnemyType::SWEEPER_A:
    {
        mainHitbox.aabb.width = 7;
        mainHitbox.aabb.height = 11;

        lookAtPlayer = true;

        maxHealth = 1;
    }
    break;

    case EnemyType::SWEEPER_B:
    {
        mainHitbox.aabb.width = 7;
        mainHitbox.aabb.height = 10;

        maxTime = 0.6f;

        lookAtPlayer = true;

        bulletData.radius = 1.0f;

        bulletData.fireRate = 1.2f;

        bulletData.speed = 80;

        bulletData.spread = 10;

        bulletData.pelletCount = 3;

        bulletData.fanShaped = true;

        bulletData.angle = -bulletData.spread;

        canFly = true;

        maxHealth = 1;
    }
    break;

    case EnemyType::AMAS_HEAVY:
    {
        lookAtPlayer = false;

        bulletData.fireRate = 0.6f;

        bulletData.radius = 2;

        bulletData.speed = 70;

        bulletData.spread = 4;

        mainHitbox.aabb.width = 20;

        mainHitbox.aabb.height = 16;

        canFly = true;

        maxHealth = 4;
    }
    break;

    case EnemyType::HELMET_GANG:
    {
        lookAtPlayer = true;

        maxTime = 1.0f;

        bulletData.speed = 80;
        bulletData.fireRate = 0.6f;

        maxHealth = 3;
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

        isBoss = true;
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

    //bulletpool = std::make_unique<BulletPool>(30, bulletData);

    enemyRenderData = GetEnemyActiveRenderData(type, characterVariantIndex);

    aiFrameskip = frameskip;

    ogDamping = gameObj.body.damping;

    ogBulletData = bulletData;

    if(canFly) gameObj.body.hasGravity = false;
    else
    {
        gameObj.body.hasGravity = true;

        spawnFlipData.flipY = gravity < 0;
    } 

    if(!randomAttack) stateTimer = 1000;
    
    if(!IsEnemyStatic(type)) spawnFlipData.flipX = false;

    gameObj.flipData = spawnFlipData;

    gameObj.UpdateHitboxes();

    health = maxHealth;
}

void Enemy::Shoot(float dt, BulletPool* bulletpool)
{
    if(!bulletpool) return;

    Vector2 bulletSpawnPos;

    switch (type)
    {
    case EnemyType::BOMBER_DRONE:
    case EnemyType::AMAS_DRONE_B:

        bulletSpawnPos = gameObj.transform.position;

    break;

    case EnemyType::AMAS_HEAVY:
    {
        bulletSpawnPos = GetTextureBulletSpawnPos(gameObj, enemyRenderData);

        int offset = 5;

        if(!gameObj.flipData.flipY)
        {
            bulletSpawnPos.y -= offset;
        }
        else
        {
            bulletSpawnPos.y += offset;
        }
    }
    break;
    
    default: bulletSpawnPos = GetTextureBulletSpawnPos(gameObj, enemyRenderData); break;
    }

    ShootBullet(
        dt, gameObj,
        bulletData, bulletSpawnPos,
        bulletpool, shooting
    );
}

void Enemy::Update(float dt, int iterations)
{
    canTakeDamage = invulTimer >= maxInvulTime;

    if(!canTakeDamage)
    {
        invulTimer += dt;

        if(invulTimer >= maxInvulTime) invulTimer = maxInvulTime;
    }

    FlipHitboxY(gameObj.hitboxes[1], gameObj.flipData.flipY, false);

    FlipHitboxY(gameObj.hitboxes[2], gameObj.flipData.flipY, true);
}