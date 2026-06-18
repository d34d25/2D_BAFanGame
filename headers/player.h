#pragma once

#include "entity.h"
#include "drawing.h"

#include "bullet.h"

#include <iostream>

#include "leveldata.h"

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

    float jumpTime = 0.0f;
    float maxJumpTime = 0.15f;

    float animationTimer = 0.0f;

    inline Vector2 GetBulletSpawnPos()
    {
        Vector2 spawnPos = {};

        Vector2 offset = weaponRenderData.offset;

        float textureWidth = weaponRenderData.sourceTexture->width;

        if(gameObj.data.flipX)
        {
            offset.x = -offset.x;

            textureWidth = -textureWidth;
        }

        if(gameObj.data.flipY)
        {
            offset.y = -offset.y;
        }

        spawnPos.x = gameObj.transform.position.x + offset.x + textureWidth;

        spawnPos.y = gameObj.transform.position.y + offset.y;

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

    bool hitCeiling = false;

    bool canMove = true;

    //jump flags
    bool isJumping = false;

    //stun
    float stunTimer = 0.0f;

    float maxStunTime = 0.0f;

    Vector2 spawnPos = {0,0};

    GameObject gameObj = {};

    SpriteRenderData characterRenderData = {};

    SpriteRenderData weaponRenderData = {};

    //EntityData entityData = {false,false};

    BulletProperties bulletData = {};

    std::unique_ptr<BulletPool> bulletpool = {};

    float ladderSnapPosX = 0.0f;

    Player(Vector2 position);

    ~Player();

    void UpdateRender(float dt);

    void Update(float dt, int iterations);

    void Shoot(float dt);

    inline Rectangle GetJumpDetector()
    {
        return gameObj.GetSubAABB(1);
    }

    inline Rectangle GetTreadmillDetector()
    {
        return gameObj.GetSubAABB(2);
    }

    inline Rectangle& GetCeilingDetector()
    {
        return gameObj.GetSubAABB(3);
    }

    inline void Respawn()
    {
        gameObj.transform.position = spawnPos;

        gameObj.body.velocity = {0,0};

        gameObj.body.altVelocity = {0,0};

        isJumping = false;
        isGrounded = false;

        canMove = true;

        stunTimer = 10000.0f;

        ResetFalgs();

        bulletpool.get()->Reset();
    }

    inline bool IsFalling()
    {
        if(gameObj.data.flipY) return gameObj.body.velocity.y <= 0;
        else return gameObj.body.velocity.y >= 0;
    }

    inline void ResetFalgs()
    {
        wasGrounded = false;
        wasTouchingGravityChanger = false;
        wasTouchingSpike = false;

        windApplied = false;

        inWater = false;
        
        inLadder = false;

        hitCeiling = false;
    }

    inline void UpdateFlags()
    {
        isGrounded = wasGrounded;
        isTouchingGravityChanger = wasTouchingGravityChanger;
        isTouchingSpike = wasTouchingSpike;
    }

    inline bool IsPressingDown()
    {
        if(!gameObj.data.flipY && IsKeyDown(KEY_DOWN)) return true;
        
        if(gameObj.data.flipY && IsKeyDown(KEY_UP)) return true;

        return false;
    }

    inline bool IsPressingUp()
    {
        if(!gameObj.data.flipY && IsKeyDown(KEY_UP)) return true;
        
        if(gameObj.data.flipY && IsKeyDown(KEY_DOWN)) return true;

        return false;
    }

    inline void ApplyStun(float duration)
    {
        if(stunTimer < maxStunTime) return;

        stunTimer = 0.0f;

        maxStunTime = duration;
    }
};