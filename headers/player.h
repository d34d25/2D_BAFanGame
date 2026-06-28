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

class Player
{

private:

    Character character = Character::MOMOI;

    float jumpTime = 0.0f;
    float maxJumpTime = 0.15f;

    float animationTimer = 0.0f;
    
public:

    int characterCurrentFrame = 0;

    int characterVariantIndex = 0;

    int weaponCurrentFrame = 0;

    int weaponVariantIndex = 0;

    float gravity = 500;

    //previous frame flags
    bool wasGrounded = false;
    bool wasTouchingGravityChanger = false;
    bool wasTouchingSpike = false;

    bool inLadder = false;
    bool climbing = false;

    //current frame flags

    bool isGrounded = false;
    bool isTouchingGravityChanger = false;
    bool isTouchingSpike = false;

    bool touchingWind = false;
    bool touchingWater = false;

    bool hitCeiling = false;

    bool canMove = true;

    //jump flags
    bool isJumping = false;

    //stun
    float stunTimer = 0.0f;

    float maxStunTime = 0.0f;

    //input

    bool movingLeft = false;
    bool movingRight = false;

    bool movingUp = false;
    bool movingDown = false;

    bool holdingUp = false;
    bool holdingDown = false;

    bool holdingJump = false;
    bool jumpingOffLadder = false;

    bool holdingShoot = false;

    bool resetingLevel = false;
    bool resetingZoom = false;

    int currentPortrait = 0;
    int currentPortraitFrame = 0;

    bool hurt = false;

    Vector2 spawnPos = {0,0};

    GameObject gameObj = {};

    SpriteRenderData* characterRenderData = {};

    SpriteRenderData* weaponRenderData = {};

    BulletProperties bulletData = {};

    std::unique_ptr<BulletPool> bulletpool = {};

    float ladderSnapPosX = 0.0f;

    Player() = default;

    void InitPlayer(Vector2 position, float gravity, bool flipY);

    ~Player();

    void UpdateInput();

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

    inline Rectangle& GetLadderDetector()
    {
        return gameObj.GetSubAABB(4);
    }

    inline void ResetInput()
    {
        movingLeft = false;
        movingRight = false;

        movingUp = false;
        movingDown = false;

        holdingUp = false;
        holdingDown = false;

        holdingJump = false;
        jumpingOffLadder = false;

        holdingShoot = false;

        resetingLevel = false;
        resetingZoom = false;
    }

    inline void Respawn()
    {
        ResetInput();

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
        isGrounded = false;
        
        isTouchingGravityChanger = false;
        isTouchingSpike = false;

        touchingWater = false;
        touchingWind = false;

        inLadder = false;

        hitCeiling = false;
    }

    inline void UpdateFlags()
    {
        wasGrounded = isGrounded;
        wasTouchingGravityChanger = isTouchingGravityChanger;
        wasTouchingSpike = isTouchingSpike;
    }

    inline bool IsHoldingDown()
    {
        if(!gameObj.data.flipY && IsKeyDown(KEY_DOWN)) return true;
        
        if(gameObj.data.flipY && IsKeyDown(KEY_UP)) return true;

        return false;
    }

    inline bool IsHoldingUp()
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