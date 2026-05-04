#include "player.h"

Player::Player(Vector2 position)
{
    spawnPos = position;

    phys.position = position;

    phys.body.hasGravity = true;

    phys.mainHitbox = {{0,0}, {20,46}};

    phys.AddSubHitbox({0,0}, {phys.GetMainAABB()->width * 0.9f, phys.GetMainAABB()->height * 0.5f});

    //for treadmills only
    phys.AddSubHitbox({0,0}, {phys.GetMainAABB()->width, phys.GetMainAABB()->height * 0.5f});

    phys.UpdateAABB();

    bulletData = {};
    bulletData.angle = 0;
    bulletData.fireTimer = 0.0f;
    bulletData.speed = 500;

    float bulletLifeTime = 2.0f;

    bool explodes = false;

    character = Character::YUZU;
    
    switch (character)
    {
    case Character::MOMOI:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-momoi.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/momoi-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -5.0f;

        weaponRenderData.offset.x = 25.0f;
        weaponRenderData.offset.y = 16.0f;

        characterRenderData.animationFrames = CropImage(
            characterRenderData.sourceTexture, 2, {12,18}
        );

        bulletData.fireRate = 0.2f;
        bulletData.spread = 8.0f;

        bulletData.radius = 4.5f;

        bulletData.mainColor = MOMOI_PINK;
        bulletData.backColor = MOMOI_PINK_BG;

        break;
    case Character::MIDORI:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-midori.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/midori-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -5.0f;

        weaponRenderData.offset.x = 29.0f;
        weaponRenderData.offset.y = 16.0f;

        bulletData.spread = 1.5f;
        bulletData.radius = 6.0f;

        bulletData.mainColor = MIDORI_GREEN;
        bulletData.backColor = MIDORI_GREEN_BG;

        break;
    case Character::YUZU:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-yuzu.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/yuzu-weapon.png");

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

        break;
    case Character::ARIS:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-aris.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/aris-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -1.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 18.0f;

        bulletData.spread = 0.0f;
        bulletData.radius = 10.0f;

        bulletData.fireRate = 1.0f;

        bulletData.mainColor = ARIS_PURPLE;
        bulletData.backColor = ARIS_PURPLE_BG;

        break;
    case Character::MOMOI_CHAQUENA:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-momoi-chaquena.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/momoi-chaquena-weapon.png");

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

    //character sprite
    SetTextureFilter(characterRenderData.sourceTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(characterRenderData.sourceTexture, TEXTURE_WRAP_CLAMP);

    //weapon sprite
    SetTextureFilter(weaponRenderData.sourceTexture, TEXTURE_FILTER_POINT);

    SetTextureWrap(weaponRenderData.sourceTexture, TEXTURE_WRAP_CLAMP);

    bulletpool = std::make_unique<BulletPool>(30, bulletLifeTime, bulletData.radius, bulletData.mainColor, bulletData.backColor, explodes);
}


void Player::Update(float dt, int iterations)
{
    float subDt = dt / iterations;

    //lateral movement

    float moveForce = 400 * phys.body.damping;

    if(IsKeyDown(KEY_LEFT))
    {
        phys.body.force.x -= moveForce;
        
        entityData.flipX = true;
    }
    else if(IsKeyDown(KEY_RIGHT))
    {
        phys.body.force.x += moveForce;

        entityData.flipX = false;
    }

    //jump

    float jumpVel = -9000;

    float jump = jumpVel;

    if(entityData.flipY) jump = -jumpVel;

    if(isJumping)
    {
        phys.body.hasGravity = false;
    }
    else
    {
        phys.body.hasGravity = true;
    }

    if(!isGrounded && std::abs(phys.body.velocity.y) <= 0.1f) isJumping = false;

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
            phys.body.velocity.y += jump * subDt;

            if(jumpTime <= 0.0f) isJumping = false;
        }
    }
    else
    {
        isJumping = false;
    }

    if(IsKeyDown(KEY_C))
    {
        currentFrame = GetCurrentFrame(characterRenderData.animationFrames, 0, 1, 5.0f);
    }

    //update
    
    phys.body.UpdateVelocity(dt, iterations, gravity);   
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

    initialVel.x = bulletData.speed * cosf(radians) + phys.body.velocity.x;
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
