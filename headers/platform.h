#pragma once

#include "raylib.h"
#include "entity.h"
#include "leveldata.h"

const Vector2 DESPAWN_LOCATION = {1000 * GRID_SIZE,  1000 * GRID_SIZE};

class Platform
{
private:

    float timer = 0.0f; 
    float maxTime = 0.3f;

    float respawnMaxTime = 0.4f;

    float ogTime = 0.0f;
    float ogRespawnTime = 0.0f;

    float rotationAngle = 0.0f;

public:

    Vector2 ogPosition = {0,0};

    GameObject gameObj = {};

    PlatformType type = PlatformType::NONE;

    int textureIndex = -1;

    int variantIndex = 0;

    bool updateRequired = false;

    float gravity = 0.0f;

    float respawnTimer = 0.0f;

    Platform() = default;

    ~Platform() = default;

    void InitPlatform(
        const Transform2D& transform,
        const EntityData& data,
        const Direction& direction,
        float gravity,
        int textureIndex,
        int variantIndex
    );

    inline void SetTimerInit(float time)
    {
        this->ogTime = time;
        this->maxTime = time;
    }

    inline void SetRespawnTimerInit(float time)
    {
        this->ogRespawnTime = time;
        this->respawnMaxTime = time;
    }

    inline bool IsInactive()
    {
        if(gameObj.transform.position.x >= DESPAWN_LOCATION.x || gameObj.transform.position.x <= -DESPAWN_LOCATION.x)
            return true;

        if(gameObj.transform.position.y >= DESPAWN_LOCATION.y || gameObj.transform.position.y <= -DESPAWN_LOCATION.y)
            return true;

        return false;
    }

    void Update(float dt, int iterations);

    void UpdateInactive(float dt, int iterations);

    inline void Respawn()
    {
        switch (type)
        {
        case PlatformType::FALLING:
        case PlatformType::DISAPPEARING:
            updateRequired = false;
        break;
        
        default:
            break;
        }

        maxTime = ogTime;
        respawnMaxTime = ogRespawnTime;

        respawnTimer = 0.0f;
        timer = 0.0f;

        gameObj.transform.position = ogPosition;

        gameObj.UpdateHitboxes();
    }
};

inline bool IsPlatformSpike(PlatformType type)
{
    switch (type)
    {
    case PlatformType::MOVING_SPIKE_HORIZONTAL: return true;
    case PlatformType::MOVING_SPIKE_VERTICAL: return true;
    case PlatformType::ROTATING_SPIKE_SINGLE: return true;
    case PlatformType::ROTATING_SPIKE_DOUBLE: return true;
    default: return false;
    }

    return false;
}