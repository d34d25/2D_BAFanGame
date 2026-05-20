#include "platform.h"
#include <iostream>

void Platform::Update(float dt, int iterations)
{
    bool isFalling = type == PlatformType::FALLING;
    bool isDisappearing = type == PlatformType::DISAPPEARING;

    bool isRotatingSpike = type == PlatformType::ROTATING_SPIKE_SINGLE || type == PlatformType::ROTATING_SPIKE_DOUBLE;

    float subDt = dt / iterations;

    if(isFalling || isDisappearing)
    {
        if(timer > 0.0f)
        {
            timer -= subDt;
            return;
        }

        if(isDisappearing)
        {
            phys.transform.position = DESPAWN_LOCATION;
        }
        else if(isFalling)
        {
            phys.body.hasGravity = true;

            phys.body.UpdateVelocity(dt, iterations, gravity);

            respawnTimer -= subDt;

            if(respawnTimer <= 0.0f)
            {
                phys.transform.position = DESPAWN_LOCATION;

                respawnTimer = 0.0f;
            }
        }
    }
    else if(isRotatingSpike)
    {
        rotationAngle = phys.body.velocity.x * 0.01f * subDt;

        float cosA = cosf(rotationAngle);
        float sinA = sinf(rotationAngle);

        float size = GRID_SIZE * 0.5f;

        for(int h = 1; h < phys.hitboxes.size(); h++)
        {
            Vector2 offset = phys.hitboxes[h].offset;

            phys.hitboxes[h].offset.x = offset.x * cosA - offset.y * sinA;
            phys.hitboxes[h].offset.y = offset.x * sinA + offset.y * cosA;
        }
    }
    else
    {
        phys.body.AddVelocities();
    }

    phys.body.UpdatePositionX(dt, iterations, &phys.transform.position.x);

    phys.body.UpdatePositionY(dt, iterations, &phys.transform.position.y);

    phys.UpdateHitboxes();
}

void Platform::UpdateInactive(float dt, int iterations)
{
    if(!IsInactive()) return;

    float subDt = dt / iterations;

    respawnTimer -= subDt;

    if(respawnTimer <= 0.0f)
    {
        timer = maxTime;

        phys.transform.position = ogPosition;

        respawnTimer = respawnMaxTime;

        phys.body.hasGravity = false;
        phys.body.velocity = {0,0};

        phys.UpdateHitboxes();

        updateRequired = false;
    }
}
