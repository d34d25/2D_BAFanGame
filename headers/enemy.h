#pragma once

#include "entity.h"

enum class EnemyType
{
    NONE,
    DUMMY
};

class Enemy
{
public:

    Vector2 spawnPosition = {0,0};

    EntityData spawnData = {false, false};

    GameObject gameObj = {};

    EnemyType type = EnemyType::NONE;

    float gravity = 0.0f;

    bool inWater = false;

    bool isActive = false;

    Enemy() = default;

    ~Enemy() = default;

    void Update(float dt, int iterations);

    inline void ResetFlags()
    {
        inWater = false;
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