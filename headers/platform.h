#pragma once

#include "raylib.h"
#include "entity.h"

const Vector2 DESPAWN_LOCATION = {1000 * gridSize,  1000 * gridSize};

enum class PlatformType
{
    NONE,
    
    FALLING,
    DISAPPEARING,

    ROTATING_SPIKE_SINGLE,
    ROTATING_SPIKE_DOUBLE,

    MOVING_START,

    MOVING_X,

    MOVING_HORIZONTAL,
    MOVING_SPIKE_HORIZONTAL,
    
    MOVING_Y,

    MOVING_VERTICAL,
    MOVING_SPIKE_VERTICAL,

    MOVING_END
};

class Platform
{
private:

    float timer = 0.3f; 
    float maxTime = 0.3f;

    float respawnTimer = 1.0f;
    float respawnMaxTime = 1.0f;

public:

    Vector2 ogPosition = {0,0};

    GameObject phys = {};

    PlatformType type = PlatformType::NONE;

    int textureIndex = -1;

    int variantIndex = 0;

    bool updateRequired = false;

    float gravity = 0.0f;

    Platform();

    ~Platform();

    inline void SetTimer(float time)
    {
        this->timer = time;
        this->maxTime = time;
    }

    inline void SetRespawnTimer(float time)
    {
        this->respawnTimer = time;
        this->respawnMaxTime = time;
    }

    inline bool IsInactive()
    {
        if(phys.transform.position.x >= DESPAWN_LOCATION.x || phys.transform.position.x <= -DESPAWN_LOCATION.x)
            return true;

        if(gravity >= 0)
        {
            if(phys.transform.position.y >= DESPAWN_LOCATION.y) return true;
        }
        else
        {
            if(phys.transform.position.y <= -DESPAWN_LOCATION.y) return true;
        }

        return false;
    }

    void Update(float dt, int iterations);

    void UpdateInactive(float dt, int iterations);
};

inline bool IsPlatformSpike(PlatformType type)
{
    switch (type)
    {
    case PlatformType::MOVING_SPIKE_HORIZONTAL: return true;
    case PlatformType::MOVING_SPIKE_VERTICAL: return true;
    
    default: return false;
    }

    return false;
}