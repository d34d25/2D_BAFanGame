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

    if(IsInactive())
    {
        updateRequired = false;
        return;
    }

    if(!updateRequired) return;

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
            updateRequired = false;
        }
        else if(isFalling)
        {
            phys.body->hasGravity = true;

            phys.body->UpdateVelocity(dt, iterations, gravity);
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