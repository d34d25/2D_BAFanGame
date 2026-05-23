#include "enemy.h"
#include <iostream>

void Enemy::Update(float dt, int iterations)
{
    
    switch (type)
    {
    case EnemyType::DUMMY:
    {
        gameObj.body.velocity.x = 20;
    }
    break;
    
    default:
        break;
    }
    
    gameObj.body.UpdateVelocity(dt, iterations, gravity);
}