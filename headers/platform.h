#pragma once

#include "raylib.h"
#include "entity.h"

const Vector2 DESPAWN_LOCATION = {-10000,  -10000};

enum class PlatformType
{
    NONE,
    MOVING_HORIZONTAL,
    MOVING_VERTICAL,
    FALLING,
    DISAPPEARING
};

class Platform
{
private:

    float timer = 0.0f; 
    float maxTime = 0.3f;

public:

    GameObject phys = {};

    //bool isHorizontal = false;
    //bool isVertical = false;

    //bool isFalling = false; 
    //bool isDisappearing = false;

    PlatformType type = PlatformType::NONE;

    int textureIndex = -1;

    int variantIndex = 0;

    bool updateRequired = false;

    float gravity = 0.0f;

    inline void SetTimer(float time)
    {
        this->timer = time;
    }

    inline bool IsInactive()
    {
        return phys.position.x == DESPAWN_LOCATION.x && phys.position.y == DESPAWN_LOCATION.y;
    }

    void Update(float dt, int iterations);
};