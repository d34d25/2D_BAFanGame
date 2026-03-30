#include "player.h"

Player::Player(Vector2 position)
{
    phys.position = position;

    phys.body.hasGravity = true;

    phys.aabb.width = 40;
    phys.aabb.height = 48;

    phys.UpdateAABB();

    characterRenderData.scale = 3.0f;

    weaponRenderData.scale = characterRenderData.scale;

    character = Character::MOMOI;
    
    switch (character)
    {
    case Character::MOMOI:

        characterRenderData.texture = LoadTexture("assets/characters/chibi-momoi.png");
        weaponRenderData.texture = LoadTexture("assets/characters/momoi-weapon.png");

        characterOffset.x = 0.0f;
        characterOffset.y = -3.0f;

        weaponOffset.x = 25.0f;
        weaponOffset.y = 16.0f;

        break;
    case Character::MIDORI:

        characterRenderData.texture = LoadTexture("assets/characters/chibi-midori.png");
        weaponRenderData.texture = LoadTexture("assets/characters/midori-weapon.png");

        characterOffset.x = 0.0f;
        characterOffset.y = -3.0f;

        weaponOffset.x = 29.0f;
        weaponOffset.y = 16.0f;

        break;
    case Character::YUZU:

        characterRenderData.texture = LoadTexture("assets/characters/chibi-yuzu.png");
        weaponRenderData.texture = LoadTexture("assets/characters/yuzu-weapon.png");

        characterOffset.x = 2.0f;
        characterOffset.y = 1.0f;

        weaponOffset.x = 8.0f;
        weaponOffset.y = 14.0f;

        break;
    case Character::ARISU:

        characterRenderData.texture = LoadTexture("assets/characters/chibi-arisu.png");
        weaponRenderData.texture = LoadTexture("assets/characters/arisu-weapon.png");

        characterOffset.x = 0.0f;
        characterOffset.y = 1.0f;

        weaponOffset.x = 16.0f;
        weaponOffset.y = 18.0f;

        break;
    case Character::MOMOI_CHAQUENA:

        characterRenderData.texture = LoadTexture("assets/characters/chibi-momoi-chaquena.png");
        weaponRenderData.texture = LoadTexture("assets/characters/momoi-chaquena-weapon.png");

        characterOffset.x = 0.0f;
        characterOffset.y = -2.0f;

        weaponOffset.x = 16.0f;
        weaponOffset.y = 16.0f;

        break;
    default:
        break;
    }

    //character sprite
    SetTextureFilter(characterRenderData.texture, TEXTURE_FILTER_POINT);

    SetTextureWrap(characterRenderData.texture, TEXTURE_WRAP_CLAMP);

    characterRenderData.sourceRect = {
        0,0, (float)characterRenderData.texture.width,
        (float)characterRenderData.texture.height
    };

    //weapon sprite
    SetTextureFilter(weaponRenderData.texture, TEXTURE_FILTER_POINT);

    SetTextureWrap(weaponRenderData.texture, TEXTURE_WRAP_CLAMP);

    weaponRenderData.sourceRect = {
        0,0, (float)weaponRenderData.texture.width,
        (float)weaponRenderData.texture.height
    };
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

    //update
    
    phys.body.UpdateVelocity(dt, iterations, gravity);
}
