#pragma once

#include "entity.h"
#include "drawing.h"

#include "bullet.h"

#include <iostream>

#include "leveldata.h"

const Color MOMOI_PINK = Color{255,97,178,255};

const Color MIDORI_GREEN = Color{154,235,0,255};

const Color YUZU_COLOR = BULLET_COLOR;

const Color ARIS_PURPLE = Color{97,211,227,255};

struct Player
{
    GameObject gameObj = {};

    Vector2 spawnPos = {0,0};

    std::unique_ptr<BulletPool> bulletpool = {};

    SpriteRenderData* characterRenderData = {};

    BulletProperties bulletData = {};

    Character character = Character::YUZU;

    float jumpTime = 0.0f;
    float maxJumpTime = 0.16f;

    float animationTimer = 0.0f;

    float gravity = 500;

    float ladderSnapPosX = 0.0f;

    //stun
    float stunTimer = 0.0f;

    float maxStunTime = 0.0f;

    //invul
    float invulTimer = 0.0f;

    float maxInvulTime = 0.0f;

    //ints
    int characterCurrentFrame = 0;

    int characterVariantIndex = 0;

    int characterCurrentPalette = 0;

    int currentPortrait = 0;

    int currentPortraitFrame = 0;

    int maxHealth = 6;

    int health = 0;

    //previous frame flags
    bool wasGrounded = false;
    bool wasTouchingGravityChanger = false;
    bool wasTouchingSpike = false;

    bool inLadder = false;

    bool climbing = false;

    bool wasHurt = false;

    //current frame flags

    bool isGrounded = false;

    bool isTouchingGravityChanger = false;

    bool isTouchingSpike = false;

    bool touchingWind = false;

    bool touchingWater = false;

    bool hitCeiling = false;

    bool canMove = true;

    bool canTakeDamage = true;

    bool hurt = false;

    //jump flags
    bool canJump = false;

    //input

    bool movingLeft = false;
    bool movingRight = false;

    bool movingUp = false;
    bool movingDown = false;

    bool holdingUp = false;
    bool holdingDown = false;

    bool holdingJump = false;

    bool jumpRleased = false;

    bool jumpingOffLadder = false;

    bool holdingShoot = false;

    bool resetingLevel = false;
    bool resetingZoom = false;

    bool eegg = false;

    bool pausePressed = false;

    bool isBossPlayingIntro = false;
    //

    bool ogFlipOffset = false;

    Player() = default;

    void InitPlayer(Vector2 position, float gravity, bool flipY);

    ~Player() = default;

    void UpdateInput();

    void UpdateRender(float dt);

    void Update(float dt);

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

        canJump = false;
        
        isGrounded = false;

        canMove = true;

        stunTimer = 10000.0f;

        ResetFalgs();

        bulletpool.get()->Reset();

        health = maxHealth;
    }

    inline bool IsFalling()
    {
        if(gameObj.flipData.flipY) return gameObj.body.velocity.y <= 0;
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

        hurt = false;
    }

    inline void UpdateFlags()
    {
        wasGrounded = isGrounded;
        wasTouchingGravityChanger = isTouchingGravityChanger;
        wasTouchingSpike = isTouchingSpike;

        wasHurt = hurt;
    }

    inline bool IsHoldingDown()
    {
        if(!gameObj.flipData.flipY && IsKeyDown(KEY_DOWN)) return true;
        
        if(gameObj.flipData.flipY && IsKeyDown(KEY_UP)) return true;

        return false;
    }

    inline bool IsHoldingUp()
    {
        if(!gameObj.flipData.flipY && IsKeyDown(KEY_UP)) return true;
        
        if(gameObj.flipData.flipY && IsKeyDown(KEY_DOWN)) return true;

        return false;
    }

    /*
        on enter trigger = !wasTouchingTrigger && isTouchingTrigger
        on exit trigger = wasTouchingTrigger && !isTouchingTrigger
    */

    inline bool TookDamage()
    {
        return canTakeDamage && hurt && !wasHurt;
    }

    inline bool TouchedSpike()
    {
        return !wasTouchingSpike && isTouchingSpike;
    }

    inline bool touchedGravityChanger()
    {
        return !wasTouchingGravityChanger && isTouchingGravityChanger;
    }

    inline void ApplyStun(float duration)
    {
        if(stunTimer < maxStunTime) return;

        stunTimer = 0.0f;

        maxStunTime = duration;
    }

    inline void ApplyInvul(float duration = 1.25f)
    {
        if(invulTimer < maxInvulTime) return;

        invulTimer = 0.0f;

        maxInvulTime = duration;
    }
};