#include "player.h"

void Player::InitPlayer(Vector2 position, float gravity, bool flipY)
{
    this->gravity = gravity;

    gameObj.flipData.flipY = flipY;

    spawnPos = position;

    gameObj.transform.position = position;

    gameObj.body = {};

    gameObj.body.hasGravity = true;

    gameObj.hitboxes.push_back(Hitbox{{0,0}, {6, 14}});

    float verticalOffset = 6.0f;

    float jumpDetectorWidth = 6.0f;

    //jump detector
    gameObj.AddSubHitbox({0,verticalOffset}, {jumpDetectorWidth, gameObj.GetMainAABB().height * 0.25f});

    //for treadmills only
    gameObj.AddSubHitbox({0,verticalOffset}, {gameObj.GetMainAABB().width, gameObj.GetMainAABB().height * 0.25f});

    //ceiling detector
    gameObj.AddSubHitbox({0,-verticalOffset}, {jumpDetectorWidth, gameObj.GetMainAABB().height * 0.25f});

    //ladder detector
    gameObj.AddSubHitbox({0,0}, {gameObj.GetMainAABB().width * 0.45f, gameObj.GetMainAABB().height + 1});

    gameObj.UpdateHitboxes();

    bulletData = {};
    bulletData.angle = 0;
    bulletData.fireTimer = 0.0f;
    bulletData.speed = 125;

    bulletData.lifeTime = 2.0f;

    bulletData.explodes = false;
    bulletData.piercing = false;

    bulletData.gravity = 0.0f;

    characterVariantIndex = 0;

    character = Character::YUZU;

    currentPortrait = -1;

    switch (character)
    {
    case Character::MOMOI:

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 1.125f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        characterCurrentPalette = 1;

        break;
    case Character::MIDORI:

        bulletData.spread = 1.5f;
        bulletData.radius = 1.5f;

        bulletData.mainColor = MIDORI_GREEN;
        bulletData.backColor = MIDORI_GREEN_BG;

        characterCurrentPalette = 2;

        break;
    case Character::YUZU:

        bulletData.gravity = 250.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 2.0f;
        bulletData.angle = -40;
        bulletData.fireRate = 1.2f;

        bulletData.mainColor = YUZU_COLOR;
        bulletData.backColor = YUZU_COLOR_BG;

        bulletData.explodes = true;

        characterCurrentPalette = 0;

        currentPortrait = 0;

        break;
    case Character::YUZU_BATTLE:

        characterVariantIndex = 1;

        bulletData.gravity = 250.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 2.0f;
        bulletData.angle = -40;
        bulletData.fireRate = 1.2f;

        bulletData.mainColor = YUZU_COLOR;
        bulletData.backColor = YUZU_COLOR_BG;

        bulletData.explodes = true;

        characterCurrentPalette = 0;
        
        break;
    case Character::ARIS:

        bulletData.spread = 0.0f;
        bulletData.radius = 2.5f;

        bulletData.fireRate = 1.0f;

        bulletData.mainColor = ARIS_PURPLE;
        bulletData.backColor = ARIS_PURPLE_BG;

        bulletData.piercing = true;

        characterCurrentPalette = 3;

        break;
    case Character::MOMOI_CHAQUENA:

        characterVariantIndex = 1;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 1.125f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    default:
        break;
    }


    characterRenderData = GetPlayerActiveRenderData(character, characterVariantIndex);

    bulletpool = std::make_unique<BulletPool>(30, bulletData);

    gameObj.flipData.flipOffset = true;

    ogFlipOffset = gameObj.flipData.flipOffset;
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

    if(IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_SHIFT))
    {
        resetingLevel = false;
        resetingZoom = true;
    }

    //release
    jumpRleased = IsKeyReleased(KEY_Z);

    hurt = IsKeyDown(KEY_W);

    eegg = IsKeyDown(KEY_S);
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

    gameObj.flipData.flipOffset = ogFlipOffset;

    currentPortraitFrame = 0;

    if(eegg)
    {
        startFrame = 10;
        endFrame = 10;
    }
    else if(!canMove || hurt)
    {
        startFrame = 9;
        endFrame = 9;

        currentPortraitFrame = 1;
    }
    else if(climbing)
    {
        gameObj.flipData.flipOffset = false;
   
        if(holdingShoot && std::abs(gameObj.body.velocity.y) <= 0.1f)
        {
            startFrame = 16;
            endFrame = 16;
        }
        else
        {
            startFrame = 7;
            endFrame = 8;
        }
    }
    else if(!isGrounded)
    {
        if(holdingShoot)
        {
            startFrame = 18;
            endFrame = 18;
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
            if(holdingShoot)
            {
                startFrame = 17;
                endFrame = 17;
            }
            else
            {
                startFrame = 5;
                endFrame = 5;
            }   
        }
    }
    else
    {
        if((movingLeft || movingRight) && std::abs(gameObj.body.velocity.x) > 1.0f)
        {
            if(holdingShoot)
            {
                startFrame = 12;
                endFrame = 15;
            }
            else
            {
                startFrame = 1;
                endFrame = 4;
            }
        }
        else
        {
            if(holdingShoot)
            {
                startFrame = 11;
                endFrame = 11;
            }
            else
            {
                startFrame = 0;
                endFrame = 0;
            }
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

void Player::Update(float dt)
{
    float jumpVel = -3500;

    float jump = jumpVel;

    if(gameObj.flipData.flipY) jump = -jumpVel;

    //jump detector
    FlipHitboxY(gameObj.hitboxes[1], gameObj.flipData.flipY, false);

    //treadmill detector
    FlipHitboxY(gameObj.hitboxes[2], gameObj.flipData.flipY, false);

    //ceiling detector
    FlipHitboxY(gameObj.hitboxes[3], gameObj.flipData.flipY, true);

    canMove = stunTimer >= maxStunTime;

    if(canMove)
    {
        //lateral movement

        float moveForce = 5000;

        if(movingLeft)
        {
            if(!climbing) gameObj.body.force.x -= moveForce;
            
            gameObj.flipData.flipX = true;
        }
        else if(movingRight)
        {
            if(!climbing) gameObj.body.force.x += moveForce;

            gameObj.flipData.flipX = false;
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

                float climbingSpeed = 50;

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
            if(isGrounded) canJump = true;

            if(hitCeiling) canJump = false;

            if(canJump && jumpTime < maxJumpTime)
            {
                gameObj.body.velocity.y += jump * dt;
            }
            else
            {
                canJump = false;
            }
        }
        else
        {
            canJump = false;
        }
    }
    else 
    {
        ResetInput();

        gameObj.body.velocity.x = 0.0f;

        stunTimer += dt;

        if(stunTimer >= maxStunTime) stunTimer = maxStunTime;
    }
}

void Player::Shoot(float dt)
{
    if(climbing && std::abs(gameObj.body.velocity.y) > 0.1f)
    {
        return;
    }

    ShootBullet(
        dt, gameObj,
        bulletData, GetTextureBulletSpawnPos(gameObj, characterRenderData),
        bulletpool.get(), holdingShoot
    );
}
