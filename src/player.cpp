#include "player.h"

Player::Player(Vector2 position)
{
    spawnPos = position;

    gameObj.transform.position = position;

    gameObj.body = {};

    gameObj.body.hasGravity = true;

    gameObj.hitboxes.push_back(Hitbox{{0,0}, {25,56}}); //20, 46

    //jump detector
    gameObj.AddSubHitbox({0,0}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    //for treadmills only
    gameObj.AddSubHitbox({0,0}, {gameObj.GetMainAABB().width, gameObj.GetMainAABB().height * 0.25f});

    gameObj.UpdateHitboxes();

    bulletData = {};
    bulletData.angle = 0;
    bulletData.fireTimer = 0.0f;
    bulletData.speed = 500;

    bulletData.lifeTime = 2.0f;//2.0f

    bulletData.explodes = false;
    bulletData.piercing = false;

    character = Character::YUZU;

    switch (character)
    {
    case Character::MOMOI:

        characterRenderData = LoadRenderData("assets/characters/momoi-spritesheet-b.png", {14,24});
        weaponRenderData = LoadRenderData("assets/characters/momoi-weapon-holo.png", {10,5});

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -7.0f;

        weaponRenderData.offset.x = 39.0f;
        weaponRenderData.offset.y = 7.0f;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    case Character::MIDORI:

        characterRenderData = LoadRenderData("assets/characters/midori-spritesheet.png", {14,24});
        weaponRenderData = LoadRenderData("assets/characters/midori-weapon-holo.png", {11,5});

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -7.0f;

        weaponRenderData.offset.x = 42.0f;
        weaponRenderData.offset.y = 7.0f;

        bulletData.spread = 1.5f;
        bulletData.radius = 6.0f;

        bulletData.mainColor = MIDORI_GREEN;
        bulletData.backColor = MIDORI_GREEN_BG;

        break;
    case Character::YUZU:

        characterRenderData = LoadRenderData("assets/characters/yuzu-spritesheet.png", {21,22});
        weaponRenderData = LoadRenderData("assets/characters/yuzu-weapon-holo.png", {10,5});

        characterRenderData.offset.x = -4.0f;
        characterRenderData.offset.y = -4.0f;

        weaponRenderData.offset.x = 42.0f;
        weaponRenderData.offset.y = 7.0f;

        bulletData.gravity = gravity * 2.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 8.0f;
        bulletData.angle = -40;
        bulletData.fireRate = 1.2f;

        bulletData.mainColor = YUZU_COLOR;
        bulletData.backColor = YUZU_COLOR_BG;

        bulletData.explodes = true;

        break;
    case Character::ARIS:

        characterRenderData = LoadRenderData("assets/characters/aris-spritesheet.png", {17,21});
        weaponRenderData = LoadRenderData("assets/characters/aris-weapon-holo.png", {15,4});

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -3.0f;

        weaponRenderData.offset.x = 51.0f;
        weaponRenderData.offset.y = 7.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 10.0f;

        bulletData.fireRate = 1.0f;

        bulletData.mainColor = ARIS_PURPLE;
        bulletData.backColor = ARIS_PURPLE_BG;

        bulletData.piercing = true;

        break;
    case Character::MOMOI_CHAQUENA:

        characterRenderData = LoadRenderData("assets/characters/chibi-momoi-chaquena.png", {14,17});
        weaponRenderData = LoadRenderData("assets/characters/momoi-chaquena-weapon.png", {26,8});

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -4.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 16.0f;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    default:
        break;
    }
    
    characterRenderData.animationSpeed = 10.0f;

    characterRenderData.ogOffset = characterRenderData.offset;

    bulletpool = std::make_unique<BulletPool>(30, bulletData);
}

Player::~Player()
{
}

void Player::UpdateRender(float dt)
{
    animationTimer += dt * characterRenderData.animationSpeed;

    /*
    idle = frame 0
    waliking = frame 1 to 3
    jumping = frame 4
    climbing = frame 5 to 6
    stunned = frame 7
    */

    if(animationTimer >= 1.0f)
    {
        animationTimer = 0.0f;

        int startFrame = 0;
        int endFrame = characterRenderData.animationFrames.size();

        characterRenderData.offset = characterRenderData.ogOffset;

        if(!canMove)
        {
            startFrame = 7;
            endFrame = 7;

            characterRenderData.offset.y += 5.0f;
        }
        else if(climbing)
        {
            startFrame = 5;
            endFrame = 6;
        }
        else if(!isGrounded)
        {
            startFrame = 4;
            endFrame = 4;
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

        if(!(climbing && gameObj.body.velocity.y == 0.0f)) currentFrame++;

        if(currentFrame > endFrame || currentFrame < startFrame) currentFrame = startFrame;

        if(currentFrame < 0) currentFrame = 0;
    }

    //std::cout<<"stun count: "<<stunCounter<<"\n";

    std::cout<<"stun timer: "<<stunTimer<<"\n";
    std::cout<<"max stun time: "<<maxStunTime<<"\n";
}

void Player::Update(float dt, int iterations)
{
    float subDt = dt / iterations;

    float jumpVel = -12000;

    float jump = jumpVel;

    if(gameObj.data.flipY) jump = -jumpVel;

    canMove = stunTimer >= maxStunTime; //stunCounter <= 0;

    if(canMove)
    {
        //lateral movement

        float moveForce = 400 * gameObj.body.damping;

        if(IsKeyDown(KEY_LEFT))
        {
            gameObj.body.force.x -= moveForce;
            
            gameObj.data.flipX = true;
        }
        else if(IsKeyDown(KEY_RIGHT))
        {
            gameObj.body.force.x += moveForce;

            gameObj.data.flipX = false;
        }

        //ladder

        if(inLadder && (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))) climbing = true;

        if(climbing)
        {
            if(!inLadder)
            {
                climbing = false;
                gameObj.body.hasGravity = true;
            }
            else
            {
                gameObj.transform.position.x = ladderSnapPosX;

                gameObj.body.hasGravity = false;

                gameObj.body.velocity = {0,0};
                gameObj.body.altVelocity = {0,0};

                float climbingSpeed = 200.0f;

                if(IsKeyDown(KEY_UP)) gameObj.body.velocity.y = -climbingSpeed;
                else if(IsKeyDown(KEY_DOWN))
                {
                    gameObj.body.velocity.y = climbingSpeed;

                    if(isGrounded)
                    {
                        climbing = false;
                        inLadder = false;
                        gameObj.body.hasGravity = true;
                    }
                } 

                if(IsKeyPressed(KEY_Z))
                {
                    climbing = false;

                    gameObj.body.velocity.y = jump * 0.1f;
                }
            }
        }
        else
        {
            gameObj.body.hasGravity = true;
        }

        //jump

        if(!isGrounded && hitCeiling) isJumping = false;

        if(isGrounded)
        {
            jumpTime = 0.0f;
        }
        else
        {
            jumpTime += subDt;

            if(jumpTime >= maxJumpTime) jumpTime = maxJumpTime; 
        }

        if(IsKeyDown(KEY_Z))
        {
            if(isGrounded) isJumping = true;

            if(isJumping && jumpTime < maxJumpTime)
            {
                gameObj.body.velocity.y += jump * subDt;
            }
            else
            {
                isJumping = false;
            }
        }
        else
        {
            isJumping = false;
        }
    }
    else 
    {
        gameObj.body.velocity = {0,0};

        stunTimer += subDt;

        if(stunTimer >= maxStunTime) stunTimer = maxStunTime;
    }
    
    //update
    
    gameObj.body.UpdateVelocity(dt, iterations, gravity);   
}

void Player::Shoot(float dt)
{
    ShootBullet(
        dt, gameObj,
        &bulletData, GetBulletSpawnPos(),
        bulletpool.get(), IsKeyDown(KEY_X)
    );
}
