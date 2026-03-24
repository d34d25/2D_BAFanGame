#include "player.h"

Player::Player(Vector2 position)
{
    phys.position = position;

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

void Player::UpdateInput()
{
    if(IsKeyDown(KEY_LEFT))
    {
        inputs[(int)Inputs::LEFT] = true;
        inputs[(int)Inputs::RIGHT] = false;
    }
    else if(IsKeyDown(KEY_RIGHT))
    {
        inputs[(int)Inputs::RIGHT] = true;
        inputs[(int)Inputs::LEFT] = false;
    }

    if(IsKeyPressed(KEY_Z)) inputs[(int)Inputs::JUMP] = true;

    if(IsKeyPressed(KEY_X)) inputs[(int)Inputs::SHOOT] = true;
}

void Player::Update(float dt, int iterations)
{
    float subDT = dt / iterations;

    //gravity
    float gravAcc = 0.0;

    gravAcc += gravity;

    phys.velocity.y += gravAcc * subDT;

    //lateral movement
    float targetSpeed = 70 * iterations;

    float acceleration = 2.0f;
    float deacceleration = 3.5f;

    float accelerationFactor = Clamp(acceleration * subDT, 0.0f, 1.0f);
    float deaccelerationFactor = Clamp(deacceleration * subDT, 0.0f, 1.0f);

    if(inputs[(int)Inputs::LEFT])
    {
        phys.velocity.x = Lerp(phys.velocity.x, -targetSpeed, accelerationFactor);
        entityData.flipX = true;
        inputs[(int)Inputs::LEFT] = false;
    }
    else if(inputs[(int)Inputs::RIGHT])
    {
        phys.velocity.x = Lerp(phys.velocity.x, targetSpeed, accelerationFactor);
        entityData.flipX = false;
        inputs[(int)Inputs::RIGHT] = false;
    }
    else
    {
        phys.velocity.x = Lerp(phys.velocity.x, 0.0f, deaccelerationFactor);
    }

    //jump

    float jumpForce = -80;

    if(inputs[(int)Inputs::JUMP]) 
    {
        phys.velocity.y = jumpForce;
        inputs[(int)Inputs::JUMP] = false;
    };


    //update
    phys.velocity.x = Clamp(phys.velocity.x, -MAX_SPEED, MAX_SPEED);
    phys.velocity.y = Clamp(phys.velocity.y, -MAX_SPEED, MAX_SPEED);

    phys.UpdateVelocity();

    phys.altVelocity = {0,0};
}
