#include "player.h"

void Player::InitPlayer(Vector2 position, float gravity, bool flipY)
{
    this->gravity = gravity;

    gameObj.data.flipY = flipY;

    spawnPos = position;

    gameObj.transform.position = position;

    gameObj.body = {};

    gameObj.body.hasGravity = true;

    gameObj.hitboxes.push_back(Hitbox{{0,0}, {25,56}}); //20, 46

    //jump detector
    gameObj.AddSubHitbox({0,23}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    //for treadmills only
    gameObj.AddSubHitbox({0,23}, {gameObj.GetMainAABB().width, gameObj.GetMainAABB().height * 0.25f});

    //ceiling detector
    gameObj.AddSubHitbox({0,-23}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.25f});

    gameObj.UpdateHitboxes();

    bulletData = {};
    bulletData.angle = 0;
    bulletData.fireTimer = 0.0f;
    bulletData.speed = 500;

    bulletData.lifeTime = 2.0f;//2.0f

    bulletData.explodes = false;
    bulletData.piercing = false;

    bulletData.gravity = 0.0f;

    variantIndex = 0;

    character = Character::YUZU;

    switch (character)
    {
    case Character::MOMOI:

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    case Character::MIDORI:

        bulletData.spread = 1.5f;
        bulletData.radius = 6.0f;

        bulletData.mainColor = MIDORI_GREEN;
        bulletData.backColor = MIDORI_GREEN_BG;

        break;
    case Character::YUZU:

        bulletData.gravity = 1000.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 8.0f;
        bulletData.angle = -40;
        bulletData.fireRate = 1.2f;

        bulletData.mainColor = YUZU_COLOR;
        bulletData.backColor = YUZU_COLOR_BG;

        bulletData.explodes = true;

        break;
    case Character::ARIS:

        bulletData.spread = 0.0f;
        bulletData.radius = 10.0f;

        bulletData.fireRate = 1.0f;

        bulletData.mainColor = ARIS_PURPLE;
        bulletData.backColor = ARIS_PURPLE_BG;

        bulletData.piercing = true;

        break;
    case Character::MOMOI_CHAQUENA:

        variantIndex = 1;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    default:
        break;
    }

    characterRenderData = GetPlayerActiveRenderData(character, variantIndex);

    weaponRenderData = GetPlayerWeaponActiveRenderData(character, variantIndex);

    bulletpool = std::make_unique<BulletPool>(30, bulletData);
}

Player::~Player()
{
}

void Player::UpdateInput()
{
    movingLeft = IsKeyDown(KEY_LEFT);

    movingRight = IsKeyDown(KEY_RIGHT);

    movingUp = IsHoldingUp(); //relative to the flipY status

    movingDown = IsHoldingDown(); //relative to the flipY status

    holdingUp = IsKeyDown(KEY_UP);

    holdingDown = IsKeyDown(KEY_DOWN);

    holdingJump = IsKeyDown(KEY_Z);

    holdingShoot = IsKeyDown(KEY_X);

    //press

    if(IsKeyPressed(KEY_Z)) jumpingOffLadder = true;

    if(IsKeyPressed(KEY_R)) resetingLevel = true;

    if(IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT)) resetingZoom = true;
}

void Player::UpdateRender(float dt)
{
    if(!characterRenderData) return;

    animationTimer += dt * characterRenderData->animationSpeed;

    /*
    idle = frame 0
    waliking = frame 1 to 3
    jumping = frame 4
    climbing = frame 5 to 6
    stunned = frame 7
    */

    int startFrame = 0;
    int endFrame = characterRenderData->animationFrames.size();

    characterRenderData->offset = characterRenderData->ogOffset;

    if(!canMove)
    {
        startFrame = 7;
        endFrame = 7;

        characterRenderData->offset.y += 5.0f;
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

    if(characterCurrentFrame > endFrame || characterCurrentFrame < startFrame)
    {
        characterCurrentFrame = startFrame;

        animationTimer = 0.0f;
    }

    if(animationTimer >= 1.0f)
    {
        animationTimer = 0.0f;
        
        if(!(climbing && gameObj.body.velocity.y == 0.0f)) characterCurrentFrame++;

        if(characterCurrentFrame > endFrame) characterCurrentFrame = startFrame;
    }

    if(characterCurrentFrame < 0) characterCurrentFrame = 0;
}

void Player::Update(float dt, int iterations)
{
    float jumpVel = -200;

    float jump = jumpVel;

    if(gameObj.data.flipY) jump = -jumpVel;

    //jump detector
    FlipHitboxY(gameObj.hitboxes[1], gameObj.data.flipY, false);

    //treadmill detector
    FlipHitboxY(gameObj.hitboxes[2], gameObj.data.flipY, false);

    //ceiling detector
    FlipHitboxY(gameObj.hitboxes[3], gameObj.data.flipY, true);

    canMove = stunTimer >= maxStunTime;

    if(canMove)
    {
        //lateral movement

        float moveForce = 16000;

        if(movingLeft)
        {
            gameObj.body.force.x -= moveForce;
            
            gameObj.data.flipX = true;
        }
        else if(movingRight)
        {
            gameObj.body.force.x += moveForce;

            gameObj.data.flipX = false;
        }

        //ladder

        if(inLadder && (holdingUp || holdingDown)) climbing = true;

        if(!inLadder) jumpingOffLadder = false;

        if(climbing)
        {
            if(!inLadder)
            {
                jumpingOffLadder = false;
                climbing = false;
                gameObj.body.hasGravity = true;
            }
            else
            {
                gameObj.transform.position.x = ladderSnapPosX;

                gameObj.body.hasGravity = false;

                gameObj.body.velocity = {0,0};
                gameObj.body.altVelocity = {0,0};

                float climbingSpeed = 200;

                if(holdingUp) gameObj.body.velocity.y = -climbingSpeed;
                else if(holdingDown)
                {
                    gameObj.body.velocity.y = climbingSpeed;

                    if(isGrounded)
                    {
                        climbing = false;
                        inLadder = false;
                        gameObj.body.hasGravity = true;
                        jumpingOffLadder = false;
                    }
                }

                if(jumpingOffLadder)
                {
                    jumpingOffLadder = false;

                    climbing = false;

                    gameObj.body.velocity.y = jump * 4;
                }
            }
        }
        else
        {
            jumpingOffLadder = false;
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
            jumpTime += dt;

            if(jumpTime >= maxJumpTime) jumpTime = maxJumpTime; 
        }

        if(holdingJump)
        {
            if(isGrounded) isJumping = true;

            if(isJumping && jumpTime < maxJumpTime)
            {
                gameObj.body.velocity.y += jump;
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
        ResetInput();

        gameObj.body.velocity = {0,0};

        stunTimer += dt;

        if(stunTimer >= maxStunTime) stunTimer = maxStunTime;
    }
}

void Player::Shoot(float dt)
{
    ShootBullet(
        dt, gameObj,
        &bulletData, GetTextureBulletSpawnPos(gameObj, weaponRenderData),
        bulletpool.get(), holdingShoot
    );
}
