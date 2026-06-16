#pragma once

#include "entity.h"
#include "bullet.h"
#include "leveldata.h"

class Enemy
{

private:

    void YuukaBehaivour(float dt, const Vector2& playerPos, Vector2* playerVel, const bool isPlayerGrounded, bool* canPlayerMove);

    int currentAttack = 0;

    int counter = 0; //for various enemies or bosses that need to count something
    //generic timer for being used inside the enemies patterns
    float timer = 0.0f;
    float maxTime = 0.2f;

    int moveSpeedSign = 1;

    float animationTimer = 0.0f;

    inline Vector2 GetBulletSpawnPos()
    {
        Vector2 spawnPos = {};

        Vector2 offset = {0,0};

        if(gameObj.data.flipX)
        {
            offset.x = -offset.x;
        }

        if(gameObj.data.flipY)
        {
            offset.y = -offset.y;
        }

        spawnPos.x = gameObj.transform.position.x + offset.x;

        spawnPos.y = gameObj.transform.position.y + offset.y;

        return spawnPos;
    }

public:

    Vector2 spawnPosition = {0,0};

    EntityData spawnData = {false, false};

    GameObject gameObj = {};

    EnemyType type = EnemyType::DUMMY;

    Color testColor = ENEMY_DUMMY;

    Rectangle roomSize = {};

    //bullets
    bool shooting = false;

    BulletProperties bulletData = {};

    std::unique_ptr<BulletPool> bulletpool = {};

    float stateTimer = 0.0f;

    float gravity = 0.0f;

    float ogGravity = 0.0f;

    //flags
    bool wasGrounded = false;

    bool isGrounded = false;

    bool isStomping = false;

    bool inWater = false;

    bool isTouchingWall = false;

    bool hitCeiling = false;

    //jump
    bool isJumping = false;

    //
    bool isActive = false;

    //animation

    //also can be understood as current frame
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

    void UpdateAI(float dt, const Vector2& playerPos, Vector2* playerVel, const bool isPlayerGrounded, bool* canPlayerMove);

    void Update(float dt, int iterations);

    void UpdateRender(float dt);

    void Shoot(float dt);

    inline Rectangle& GetJumpDetector()
    {
        float offset = 5.0f;

        int dir = gameObj.data.flipY ? -1 : 1;

        float centerY = gameObj.GetMainAABB().y + gameObj.GetMainAABB().height * 0.5f;

        if(dir == 1) gameObj.GetSubAABB(1).y = centerY + offset;
        else gameObj.GetSubAABB(1).y = centerY - offset - gameObj.GetSubAABB(1).height;
        
        return gameObj.GetSubAABB(1);
    }

    inline Rectangle& GetCeilingDetector()
    {
        float offset = 1.0f;

        int dir = gameObj.data.flipY ? 1 : -1;

        float centerY = gameObj.GetMainAABB().y + gameObj.GetMainAABB().height * 0.5f;

        if(dir == 1) gameObj.GetSubAABB(1).y = centerY + offset;
        else gameObj.GetSubAABB(1).y = centerY - offset - gameObj.GetSubAABB(1).height;
        
        return gameObj.GetSubAABB(1);
    }

    inline void ResetFlags()
    {
        inWater = false;
    }

    inline void ResetFlagsAI()
    {
        isTouchingWall = false;
        wasGrounded = false;
        hitCeiling = false;
    }

    inline void UpdateFlags()
    {
        isGrounded = wasGrounded;
    }

    inline void Respawn()
    {
        gameObj.transform.position = spawnPosition;
        gameObj.body.velocity = {0,0};
        gameObj.body.altVelocity = {0,0};

        gameObj.data = spawnData;

        ResetFlags();

        isStomping = false;

        gameObj.UpdateHitboxes();

        bulletpool.get()->Reset();
    }
};