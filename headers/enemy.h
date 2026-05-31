#pragma once

#include "entity.h"

enum class EnemyType
{
    DUMMY,
    YUUKA
};

class Enemy
{

private:

    void YuukaBehaivour(float dt, const Vector2& playerPos);

    int currentAttack = 0;

    float stateTimer = 0.0f;

    int counter = 0; //for various enemies or bosses that need to count something
    //generic timer for being used inside the enemies patterns
    float timer = 0.0f;
    float maxTime = 0.2f;

public:

    Vector2 spawnPosition = {0,0};

    EntityData spawnData = {false, false};

    GameObject gameObj = {};

    EnemyType type = EnemyType::DUMMY;

    Color testColor = ENEMY_DUMMY;

    float gravity = 0.0f;

    //flags
    bool wasGrounded = false;

    bool isGrounded = false;

    bool inWater = false;

    //jump
    bool isJumping = false;
    float jumpTime = 0.0f;
    float maxJumpTime = 0.3f;

    //
    bool isActive = false;

    Enemy() = default;

    ~Enemy() = default;

    void UpdateAI(float dt, const Vector2& playerPos);

    void Update(float dt, int iterations);

    inline Rectangle& GetJumpDetector()
    {
        float offset = 5.0f;

        int dir = gameObj.data.flipY ? -1 : 1;

        float centerY = gameObj.GetMainAABB().y + gameObj.GetMainAABB().height * 0.5f;

        if(dir == 1) gameObj.GetSubAABB(1).y = centerY + offset;
        else gameObj.GetSubAABB(1).y = centerY - offset - gameObj.GetSubAABB(1).height;
        
        return gameObj.GetSubAABB(1);
    }

    inline void ResetFlags()
    {
        inWater = false;
        wasGrounded = false;
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
    }
};