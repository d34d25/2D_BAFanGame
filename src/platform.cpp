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
            gameObj.transform.position = DESPAWN_LOCATION;
        }
        else if(isFalling)
        {
            gameObj.body.hasGravity = true;

            gameObj.body.UpdateVelocity(dt, iterations, gravity);

            respawnTimer -= subDt;

            if(respawnTimer <= 0.0f)
            {
                gameObj.transform.position = DESPAWN_LOCATION;

                respawnTimer = 0.0f;
            }
        }
    }
    else if(isRotatingSpike)
    {
        rotationAngle = gameObj.body.velocity.x * 0.01f * subDt;

        float cosA = cosf(rotationAngle);
        float sinA = sinf(rotationAngle);

        float size = GRID_SIZE * 0.5f;

        for(int h = 1; h < gameObj.hitboxes.size(); h++)
        {
            Vector2 offset = gameObj.hitboxes[h].offset;

            gameObj.hitboxes[h].offset.x = offset.x * cosA - offset.y * sinA;
            gameObj.hitboxes[h].offset.y = offset.x * sinA + offset.y * cosA;
        }
    }
    else
    {
        gameObj.body.AddVelocities();
    }

    gameObj.body.UpdatePositionX(dt, iterations, &gameObj.transform.position.x);

    gameObj.body.UpdatePositionY(dt, iterations, &gameObj.transform.position.y);

    gameObj.UpdateHitboxes();
}

void Platform::UpdateInactive(float dt, int iterations)
{
    if(!IsInactive()) return;

    float subDt = dt / iterations;

    respawnTimer -= subDt;

    if(respawnTimer <= 0.0f)
    {
        timer = maxTime;

        gameObj.transform.position = ogPosition;

        respawnTimer = respawnMaxTime;

        gameObj.body.hasGravity = false;
        gameObj.body.velocity = {0,0};

        gameObj.UpdateHitboxes();

        updateRequired = false;
    }
}
