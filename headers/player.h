#pragma once

#include "entity.h"
#include "drawing.h"

#include<iostream>

enum class Character
{
    MOMOI,
    MIDORI,
    YUZU,
    ARISU,
    MOMOI_CHAQUENA
};

class Player
{

private:

    Character character = Character::MOMOI;

    Vector2 characterOffset = {0,0};

    Vector2 weaponOffset = {0,0};

    float jumpTime = 0.0f;
    float maxJumpTime = 0.15f;
    
public:

    int currentFrame = 0;

    float gravity = 500;

    bool canJump = false;
    bool isJumping = false;

    bool isTouchingGravityChanger = false;
    bool isTouchingSpike = false;

    Vector2 spawnPos = {0,0};

    GameObject phys = {};

    SpriteRenderData characterRenderData = {};

    SpriteRenderData weaponRenderData = {};

    EntityData entityData = {false,false};

    Player(Vector2 position);

    void Update(float dt, int iterations);

    inline void UpdateRender()
    {
        UpdateSprite(&characterRenderData, entityData, phys.position, characterOffset);

        UpdateSprite(&weaponRenderData, entityData, phys.position, weaponOffset);
    }

    inline Rectangle GetJumpDetector()
    {
        float offset = 5.0f;

        int dir = entityData.flipY ? -1 : 1;

        float centerY = phys.mainAABB.y + phys.mainAABB.height * 0.5f;

        if(dir == 1) phys.subAABBList[0].y = centerY + offset;
        else phys.subAABBList[0].y = centerY - offset - phys.subAABBList[0].height;
        
        return phys.subAABBList[0];
    }

    inline void Respawn()
    {
        phys.position = spawnPos;

        phys.body.velocity = {0,0};

        phys.body.altVelocity = {0,0};

        isJumping = false;
        canJump = false;
    }
};