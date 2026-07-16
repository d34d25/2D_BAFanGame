#include "entity.h"

void SimpleBody2D::UpdateVelocity(float dt, int iterations, float gravity)
{
    float subDt = dt / iterations;

    acceleration = {0,0};

    acceleration.x += force.x / MASS;
    acceleration.y += force.y / MASS;

    if(hasGravity) acceleration.y += gravity;
    
    float decay = expf(-damping * subDt);

    if(damping > 0.001f)
    {
        velocity.x = (velocity.x * decay) + (acceleration.x / damping) * (1.0f - decay);
    }
    else
    {
        velocity.x += acceleration.x * subDt;
    }

    if(damping > 0.001f)
    {
        velocity.y = (velocity.y * decay) + (acceleration.y / damping) * (1.0f - decay);
    }
    else
    {
        velocity.y += acceleration.y * subDt;
    }

    velocity.x = Clamp(velocity.x ,-MAX_SPEED.x, MAX_SPEED.x);

    velocity.y = Clamp(velocity.y ,-MAX_SPEED.y, MAX_SPEED.y);

    altVelocity.x = Clamp(altVelocity.x ,-MAX_SPEED.x, MAX_SPEED.x);

    altVelocity.y = Clamp(altVelocity.y ,-MAX_SPEED.y, MAX_SPEED.y);

    AddVelocities();

    force = {0,0};

    altVelocity = {0,0};
}