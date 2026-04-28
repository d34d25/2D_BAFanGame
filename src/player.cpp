#include "player.h"

Player::Player(Vector2 position)
{
    spawnPos = position;

    phys.position = position;

    phys.body.hasGravity = true;

    phys.mainHitbox = {{0,0}, {20,46}};

    phys.AddSubHitbox({0,0}, {phys.GetMainAABB()->width * 0.85f, phys.GetMainAABB()->height * 0.5f});

    phys.UpdateAABB();

    //characterRenderData.scale = 3.0f;

    //weaponRenderData.scale = characterRenderData.scale;

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

        break;
    case Character::MIDORI:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-midori.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/midori-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -5.0f;

        weaponRenderData.offset.x = 29.0f;
        weaponRenderData.offset.y = 16.0f;

        break;
    case Character::YUZU:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-yuzu.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/yuzu-weapon.png");

        characterRenderData.offset.x = -4.0f;
        characterRenderData.offset.y = -1.0f;

        weaponRenderData.offset.x = 8.0f;
        weaponRenderData.offset.y = 14.0f;

        break;
    case Character::ARISU:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-arisu.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/arisu-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -1.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 18.0f;

        break;
    case Character::MOMOI_CHAQUENA:

        characterRenderData.sourceTexture = LoadTexture("assets/characters/chibi-momoi-chaquena.png");
        weaponRenderData.sourceTexture = LoadTexture("assets/characters/momoi-chaquena-weapon.png");

        characterRenderData.offset.x = 0.0f;
        characterRenderData.offset.y = -4.0f;

        weaponRenderData.offset.x = 16.0f;
        weaponRenderData.offset.y = 16.0f;

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
}


void Player::Update(float dt, int iterations)
{
    float subDt = dt / iterations;

    //lateral movement

    float moveForce = 400 * phys.body.damping;

    if(IsKeyDown(KEY_LEFT))
    {
        phys.body.force.x = -moveForce;
        
        entityData.flipX = true;
    }
    else if(IsKeyDown(KEY_RIGHT))
    {
        phys.body.force.x = moveForce;

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

    if(!canJump && std::abs(phys.body.velocity.y) <= 0.1f) isJumping = false;

    if(canJump)
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
        if(canJump) isJumping = true;

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