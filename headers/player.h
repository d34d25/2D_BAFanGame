#pragma once

#include "entity.h"
#include "drawing.h"

#include "bullet.h"

#include <iostream>

#include "platform.h"

const Color MOMOI_PINK = Color{255,30,150,255};
const Color MOMOI_PINK_BG = Color{255,180,230,255};

const Color MIDORI_GREEN = Color{120,230,10,255};
const Color MIDORI_GREEN_BG = YELLOW;

const Color YUZU_COLOR = GOLD;
const Color YUZU_COLOR_BG = BLACK;

const Color ARIS_PURPLE = Color{120,100,255,255};
const Color ARIS_PURPLE_BG = Color{200,162,255,255};

enum class Character
{
    MOMOI,
    MIDORI,
    YUZU,
    ARIS,
    MOMOI_CHAQUENA
};

class Player
{

private:

    Character character = Character::MOMOI;

    Texture2D playerTexture = {};
    Texture2D weaponTexture = {};

    float jumpTime = 0.0f;
    float maxJumpTime = 0.15f;

    inline Vector2 GetBulletSpawnPos()
    {
        Vector2 spawnPos = {};

        Vector2 offset = weaponRenderData.offset;

        float textureWidth = weaponRenderData.sourceTexture->width;

        if(entityData.flipX)
        {
            offset.x = -offset.x;

            textureWidth = -textureWidth;
        }

        if(entityData.flipY)
        {
            offset.y = -offset.y;
        }

        spawnPos.x = phys.transform.position.x + offset.x + textureWidth;

        spawnPos.y = phys.transform.position.y + offset.y;

        return spawnPos;
    }
    
public:

    int currentFrame = 0;

    float gravity = 500;

    //previous frame flags
    bool wasGrounded = false;
    bool wasTouchingGravityChanger = false;
    bool wasTouchingSpike = false;

    bool windApplied = false;

    bool inWater = false;

    bool inLadder = false;
    bool climbing = false;

    //current frame flags
    bool isGrounded = false;
    bool isTouchingGravityChanger = false;
    bool isTouchingSpike = false;

    //jump flags
    bool isJumping = false;

    Vector2 spawnPos = {0,0};

    GameObject phys = {};

    SpriteRenderData characterRenderData = {};

    SpriteRenderData weaponRenderData = {};

    EntityData entityData = {false,false};

    BulletProperties bulletData = {};

    std::unique_ptr<BulletPool> bulletpool = {};

    std::vector<Platform*> platformCache_physics = {};
    std::vector<Platform*> platformCache_rendering = {};

    float laddedSnapPosX = 0.0f;

    Player(Vector2 position);

    ~Player();

    void Update(float dt, int iterations);

    void Shoot(float dt);

    inline Rectangle GetJumpDetector()
    {
        float offset = 5.0f;

        int dir = entityData.flipY ? -1 : 1;

        float centerY = phys.GetMainAABB().y + phys.GetMainAABB().height * 0.5f;

        if(dir == 1) phys.GetSubAABB(1).y = centerY + offset;
        else phys.GetSubAABB(1).y = centerY - offset - phys.GetSubAABB(1).height;
        
        return phys.GetSubAABB(1);
    }

    inline Rectangle GetTreadmillDetector()
    {
        float offset = 5.0f;

        int dir = entityData.flipY ? -1 : 1;

        float centerY = phys.GetMainAABB().y + phys.GetMainAABB().height * 0.5f;

        if(dir == 1) phys.GetSubAABB(2).y = centerY + offset;
        else phys.GetSubAABB(2).y = centerY - offset - phys.GetSubAABB(2).height;
        
        return phys.GetSubAABB(2);
    }

    inline void Respawn()
    {
        phys.transform.position = spawnPos;

        phys.body.velocity = {0,0};

        phys.body.altVelocity = {0,0};

        isJumping = false;
        isGrounded = false;

        ResetFalgs();
    }

    inline bool IsFalling()
    {
        if(entityData.flipY) return phys.body.velocity.y <= 0;
        else return phys.body.velocity.y >= 0;
    }

    inline void ResetFalgs()
    {
        wasGrounded = false;
        wasTouchingGravityChanger = false;
        wasTouchingSpike = false;

        windApplied = false;

        inWater = false;
        
        inLadder = false;
    }

    inline void UpdateFlags()
    {
        isGrounded = wasGrounded;
        isTouchingGravityChanger = wasTouchingGravityChanger;
        isTouchingSpike = wasTouchingSpike;
    }

    inline bool IsPressingDown()
    {
        if(!entityData.flipY && IsKeyDown(KEY_DOWN)) return true;
        
        if(entityData.flipY && IsKeyDown(KEY_UP)) return true;

        return false;
    }

    inline bool IsPressingUp()
    {
        if(!entityData.flipY && IsKeyDown(KEY_UP)) return true;
        
        if(entityData.flipY && IsKeyDown(KEY_DOWN)) return true;

        return false;
    }
};