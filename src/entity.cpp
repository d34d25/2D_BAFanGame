#include "entity.h"

void SimpleBody2D::UpdateVelocity(float dt, int iterations, float gravity)
{
    float subDt = dt / iterations;

    acceleration = {0,0};

    acceleration.x += force.x / MASS;
    acceleration.y += force.y / MASS;

    if(hasGravity) acceleration.y += gravity;
        
    velocity.x += acceleration.x * subDt;
    velocity.y += acceleration.y * subDt;

    float decay = expf(-damping * subDt);

    velocity = Vector2Scale(velocity, decay);

    finalVelocity = Vector2Add(velocity, altVelocity);

    force = {0,0};

    altVelocity = {0,0};
}