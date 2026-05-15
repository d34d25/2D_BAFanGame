#include "platform.h"
#include <iostream>

Platform::Platform()
{
    phys.body = new SimpleBody2D();
}

Platform::~Platform()
{
    if(phys.body)
    {
        delete phys.body;
        phys.body = nullptr;
    }
}

void Platform::Update(float dt, int iterations)
{
    bool isFalling = type == PlatformType::FALLING;
    bool isDisappearing = type == PlatformType::DISAPPEARING;

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
            phys.body->hasGravity = true;

            phys.body->UpdateVelocity(dt, iterations, gravity);

            respawnTimer -= subDt;

            if(respawnTimer <= 0.0f)
            {
                phys.transform.position = DESPAWN_LOCATION;

                respawnTimer = 0.0f;
            }
        }
    }
    else
    {
        phys.body->AddVelocities();
    }

    phys.body->UpdatePositionX(dt, iterations, &phys.transform.position.x);

    phys.body->UpdatePositionY(dt, iterations, &phys.transform.position.y);

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

        phys.body->hasGravity = false;
        phys.body->velocity = {0,0};

        phys.UpdateHitboxes();

        updateRequired = false;
    }
}
