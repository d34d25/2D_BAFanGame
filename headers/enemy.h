#pragma once

#include "entity.h"
#include "bullet.h"
#include "leveldata.h"

#include "player.h"

enum class StunState
{
    STUNNED,
    NOT_STUNNED,
    DODGED
};

enum class Attacks
{
    NOTHING,
    STOMP,
    RUN_N_SHOOT,
    WIP
};

class Enemy
{

private:

    void YuukaBehaivour(float dt, int framskip, Player& player);

    Attacks currentAttack = Attacks::WIP;

    //generic timer for being used inside the enemies patterns
    float timer = 0.0f;
    float maxTime = 0.2f;

    int moveSpeedSign = 1;

    float animationTimer = 0.0f;

    //can the enemy stun the player?
    StunState stunState = StunState::NOT_STUNNED;

    bool alreadyFlipped = false;

public:

    Vector2 spawnPosition = {0,0};

    EntityData spawnData = {false, false};

    GameObject gameObj = {};

    EnemyType type = EnemyType::DUMMY;

    Color testColor = ENEMY_DUMMY;

    Rectangle roomSize = {};

    SpriteRenderData* enemyRenderData = nullptr;

    SpriteRenderData* weaponRenderData = nullptr;

    //bullets
    bool shooting = false;

    BulletProperties bulletData = {};

    std::unique_ptr<BulletPool> bulletpool = {};

    float stateTimer = 0.0f;

    float gravity = 0.0f;

    float ogGravity = 0.0f;

    //flags
    bool isGrounded = false;

    bool wasGrounded = false;

    bool isStomping = false;

    bool isStompingRender = false;

    bool inWater = false;

    bool isTouchingWall = false;

    bool hitCeiling = false;

    bool justLanded = false;

    //jump
    bool isJumping = false;

    //
    bool isActive = false;

    //animation

    int currentFrame = 0;

    int variantIndex = 0;

    Enemy() = default;

    ~Enemy() = default;

    Enemy(Enemy&& other) noexcept = default;

    void InitEnemy(
        const Vector2& spawnPos,
        const EntityData& data,
        float gravity
    );

    void UpdateAI(float dt, int framskip, Player& player);

    void Update(float dt, int iterations);

    void UpdateRender(float dt);

    void Shoot(float dt);

    inline Rectangle& GetJumpDetector()
    {
        return gameObj.GetSubAABB(1);
    }

    inline Rectangle& GetCeilingDetector()
    {
        return gameObj.GetSubAABB(2);
    }

    inline void ResetFlags()
    {
        inWater = false;
    }

    inline void ResetFlagsAI()
    {
        isTouchingWall = false;
        isGrounded = false;
        hitCeiling = false;
    }

    inline void UpdateFlags()
    {
        justLanded = isGrounded && !wasGrounded;

        wasGrounded = isGrounded;
    }

    inline void Respawn()
    {
        gameObj.transform.position = spawnPosition;
        gameObj.body.velocity = {0,0};
        gameObj.body.altVelocity = {0,0};

        gameObj.data = spawnData;

        ResetFlags();

        isStomping = false;

        isStompingRender = false;

        gameObj.UpdateHitboxes();

        bulletpool.get()->Reset();
    }
};