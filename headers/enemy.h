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

    //YUUKA
    STOMP,
    STOMP_N_SHOT,
    RUN_N_SHOOT
};

struct Enemy
{
    GameObject gameObj = {};
    
    BulletProperties ogBulletData = {};
    
    BulletProperties bulletData = {};

    Rectangle roomSize = {};

    SpriteRenderData* enemyRenderData = nullptr;

    Vector2 spawnPosition = {0,0};

    Vector2 targetPos = {0,0};

    std::unique_ptr<BulletPool> bulletpool = {};

    Color testColor = ENEMY_DUMMY;

    SpriteFlipData spawnFlipData = {false, false};

    float stateTimer = 0.0f;

    float gravity = 0.0f;

    float ogGravity = 0.0f;

    float ogDamping = 0.0f;

    float jumpTimer = 0.0f;

    float maxJumpTime = 0.0f;

    //generic timer for being used inside the enemies patterns
    float timer = 0.0f;

    float maxTime = 0.2f;
    //
    float animationTimer = 0.0f;

    float invulTimer = 0.0f;

    float maxInvulTime = 0.0f;

    float introTimer = 0.0f;

    float maxIntroTime = 0.0f;

    EnemyType type = EnemyType::DUMMY;

    //can the enemy stun the player?
    StunState stunState = StunState::NOT_STUNNED;

    Attacks currentAttack = Attacks::NOTHING;

    Attacks lastAttack = Attacks::NOTHING;

    //animation

    int characterCurrentFrame = 0;

    int characterVariantIndex = 0;

    int characterPaletteIndex = 0;

    //
    int moveSpeedSign = 1;
    
    int aiFrameskip = 2; //2

    int maxHealth = 6;

    int health = 0;

    //flags
    bool alreadyFlipped = false;

    bool lookAtPlayer = false;

    bool isGrounded = false;

    bool wasGrounded = false;

    bool genericCondition = false;

    bool inWater = false;

    bool isTouchingWall = false;

    bool hitCeiling = false;

    bool justLanded = false;

    bool hurt = false;

    bool wasHurt = false;

    bool canTakeDamage = false;

    //jump
    bool canJump = false;

    //bullets
    bool shooting = false;

    //
    bool isActive = false;

    bool canFly = false;

    bool randomAttack = false;

    bool isBoss = false;

    bool playingIntro = false;

    Enemy() = default;

    ~Enemy() = default;

    Enemy(Enemy&& other) noexcept = default;

    void InitEnemy(
        const Vector2& spawnPos,
        const SpriteFlipData& data,
        int paletteIndex,
        float gravity,
        int frameskip = 2
    );

    void AmasDroneBehavior(float dt, Player& player);

    void AmasDroneBBehavior(float dt, Player& player);

    void BomberDroneBehavior(float dt, Player& player);

    void SweeperABehavior(float dt, Player& player);

    void SweeperBBehavior(float dt, Player& player);

    void HelmetGangBehavior(float dt, Player& player);

    void AmasHeavyBehavior(float dt, Player& player);

    void YuukaBehavior(float dt, Player& player);

    void UpdateAI(float dt, Player& player);

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

        hurt = false;
    }

    inline void UpdateFlags()
    {
        wasHurt = hurt;
    }

    inline void ResetFlagsAI()
    {
        isTouchingWall = false;
        isGrounded = false;
        hitCeiling = false;
    }

    inline void UpdateFlagsAI()
    {
        justLanded = isGrounded && !wasGrounded;

        wasGrounded = isGrounded;
    }

    inline void Respawn()
    {
        gameObj.transform.position = spawnPosition;
        gameObj.body.velocity = {0,0};
        gameObj.body.altVelocity = {0,0};

        gameObj.flipData = spawnFlipData;

        ResetFlags();

        genericCondition = false;

        gameObj.UpdateHitboxes();

        bulletpool.get()->Reset();

        health = maxHealth;

        playingIntro = false;

        if(isBoss) StartIntro();
    }

    inline bool TookDamage()
    {
        return hurt && !wasHurt && canTakeDamage;
    }

    inline void ApplyInvul(float duration = 1.25f)
    {
        if(invulTimer < maxInvulTime) return;

        invulTimer = 0.0f;

        maxInvulTime = duration;
    }

    inline void StartIntro(float duration = 0.5f)
    {
        if(introTimer < maxIntroTime) return;

        introTimer = 0.0f;

        maxIntroTime = duration;
    }

    inline void PlayIntro(float dt)
    {
        playingIntro = introTimer < maxIntroTime;

        if(playingIntro)
        {
            introTimer += dt;

            if(introTimer >= maxIntroTime)
                introTimer = maxIntroTime;
        }
    }
};