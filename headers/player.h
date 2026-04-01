#pragma once

#include "entity.h"
#include <array>

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

    Rectangle jumpDetector = {0,0,1,1};

    Character character = Character::MOMOI;

    Vector2 characterOffset = {0,0};

    Vector2 weaponOffset = {0,0};

    float jumpTime = 0.0f;
    float maxJumpTime = 0.15f;

public:

    float gravity = 500;

    bool canJump = true;
    bool isJumping = false;

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

        jumpDetector.height = phys.aabb.height * 0.5f;
        jumpDetector.width = phys.aabb.width * 0.85f;

        jumpDetector.x = phys.aabb.x + (phys.aabb.width - jumpDetector.width) * 0.5f;

        float centerY = phys.aabb.y + phys.aabb.height * 0.5f;

        if(dir == 1) jumpDetector.y = centerY + offset;
        else jumpDetector.y = centerY - offset - jumpDetector.height;
        
        return jumpDetector;
    }
};