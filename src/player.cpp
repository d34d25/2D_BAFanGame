#include "player.h"

Player::Player(Vector2 position)
{
    spawnPos = position;

    gameObj.transform.position = position;

    gameObj.body = {};

    gameObj.body.hasGravity = true;

    gameObj.hitboxes.push_back(Hitbox{{0,0}, {20,46}});

    //jump detector
    gameObj.AddSubHitbox({0,0}, {gameObj.GetMainAABB().width * 0.9f, gameObj.GetMainAABB().height * 0.5f});

    //for treadmills only
    gameObj.AddSubHitbox({0,0}, {gameObj.GetMainAABB().width, gameObj.GetMainAABB().height * 0.5f});

    gameObj.UpdateHitboxes();

    bulletData = {};
    bulletData.angle = 0;
    bulletData.fireTimer = 0.0f;
    bulletData.speed = 500;

    float bulletLifeTime = 2.0f;//2.0f

    bool explodes = false;

    Vector2 characterFrameSize = {0,0};

    Vector2 weaponFrameSize = {0,0};

    character = Character::YUZU;

    switch (character)
    {
    case Character::MOMOI:

        playerTexture = LoadTexture("assets/characters/chibi-momoi.png");
        weaponTexture = LoadTexture("assets/characters/momoi-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -5.0f;

        weaponRenderData.offset.x = 25.0f;
        weaponRenderData.offset.y = 16.0f;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        characterFrameSize = {12,18};

        weaponFrameSize = {32,8};

        break;
    case Character::MIDORI:

        playerTexture = LoadTexture("assets/characters/chibi-midori.png");
        weaponTexture = LoadTexture("assets/characters/midori-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -5.0f;

        weaponRenderData.offset.x = 29.0f;
        weaponRenderData.offset.y = 16.0f;

        bulletData.spread = 1.5f;
        bulletData.radius = 6.0f;

        bulletData.mainColor = MIDORI_GREEN;
        bulletData.backColor = MIDORI_GREEN_BG;

        characterFrameSize = {12,18};

        weaponFrameSize = {32,10};

        break;
    case Character::YUZU:

        playerTexture = LoadTexture("assets/characters/chibi-yuzu.png");
        weaponTexture = LoadTexture("assets/characters/yuzu-weapon.png");

        characterRenderData.offset.x = -4.0f;
        characterRenderData.offset.y = -1.0f;

        weaponRenderData.offset.x = 8.0f;
        weaponRenderData.offset.y = 14.0f;

        bulletData.gravity = gravity;

        bulletData.spread = 0.0f;
        bulletData.radius = 8.0f;
        bulletData.angle = -80;
        bulletData.fireRate = 1.2f;

        bulletData.mainColor = YUZU_COLOR;
        bulletData.backColor = YUZU_COLOR_BG;

        explodes = true;

        characterFrameSize = {14,15};

        weaponFrameSize = {19,10};

        break;
    case Character::ARIS:

        playerTexture = LoadTexture("assets/characters/chibi-aris.png");
        weaponTexture = LoadTexture("assets/characters/aris-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -1.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 18.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 10.0f;

        bulletData.fireRate = 1.0f;

        bulletData.mainColor = ARIS_PURPLE;
        bulletData.backColor = ARIS_PURPLE_BG;

        characterFrameSize = {12,15};

        weaponFrameSize = {33,10};

        break;
    case Character::MOMOI_CHAQUENA:

        playerTexture = LoadTexture("assets/characters/chibi-momoi-chaquena.png");
        weaponTexture = LoadTexture("assets/characters/momoi-chaquena-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -4.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 16.0f;

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        characterFrameSize = {14,17};

        weaponFrameSize = {26,8};

        break;
    default:
        break;
    }

    //character sprite
    SetTextureFilter(playerTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(playerTexture, TEXTURE_WRAP_CLAMP);

    //weapon sprite
    SetTextureFilter(weaponTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(weaponTexture, TEXTURE_WRAP_CLAMP);

    characterRenderData.sourceTexture = &playerTexture;
    weaponRenderData.sourceTexture = &weaponTexture;

    characterRenderData.animationFrames = CropImage(*characterRenderData.sourceTexture, characterFrameSize);

    weaponRenderData.animationFrames = CropImage(*weaponRenderData.sourceTexture, weaponFrameSize);

    bulletpool = std::make_unique<BulletPool>(30, bulletLifeTime, bulletData.radius, bulletData.mainColor, bulletData.backColor, explodes);
}

Player::~Player()
{
}

void Player::Update(float dt, int iterations)
{
    float subDt = dt / iterations;

    float jumpVel = -9000;

    float jump = jumpVel;

    if(entityData.flipY) jump = -jumpVel;

    //lateral movement

    float moveForce = 400 * gameObj.body.damping;

    if(canMove)
    {
        if(IsKeyDown(KEY_LEFT))
        {
            gameObj.body.force.x -= moveForce;
            
            entityData.flipX = true;
        }
        else if(IsKeyDown(KEY_RIGHT))
        {
            gameObj.body.force.x += moveForce;

            entityData.flipX = false;
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
                gameObj.transform.position.x = laddedSnapPosX;

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

        if(isJumping)
        {
            gameObj.body.hasGravity = false;
        }
        else if(!isJumping && !climbing)
        {
            gameObj.body.hasGravity = true;
        }

        if(!isGrounded && std::abs(gameObj.body.velocity.y) <= 0.1f) isJumping = false;

        if(isGrounded)
        {
            jumpTime = maxJumpTime;
        }
        else
        {
            jumpTime -= subDt;

            if(jumpTime <= 0.0f) jumpTime = 0.0f; 
        }

        if(IsKeyDown(KEY_Z))
        {
            if(isGrounded) isJumping = true;

            if(isJumping)
            {
                gameObj.body.velocity.y += jump * subDt;

                if(jumpTime <= 0.0f) isJumping = false;
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
    }
    
    //update
    
    gameObj.body.UpdateVelocity(dt, iterations, gravity);   
}

void Player::Shoot(float dt)
{
    float angle = bulletData.angle;

    float bulletGravity = bulletData.gravity;

    if(entityData.flipX)
    {
        angle = 180.0f - angle;
    }

    if(entityData.flipY)
    {
        angle = -angle;
        bulletGravity = -bulletData.gravity;
    }

    float radians = GenerateBulletSpread(angle, bulletData.spread) * (PI / 180.0f);

    Vector2 initialVel = {0,0};

    initialVel.x = bulletData.speed * cosf(radians) + gameObj.body.velocity.x;
    initialVel.y = bulletData.speed * sinf(radians);

    if(bulletData.fireTimer > 0.0f) bulletData.fireTimer -= dt;

    if(IsKeyDown(KEY_X))
    {
        while((bulletData.fireTimer <= 0.0f))
        {
            bulletpool.get()->FireBullet(GetBulletSpawnPos(), initialVel, bulletGravity);

            bulletData.fireTimer = bulletData.fireRate;
        }
    }
}
