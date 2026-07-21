#include "bullet.h"

BulletPool::BulletPool(int quantity, const BulletProperties &bulletData)
{
    explodes = bulletData.explodes;

    pierces = bulletData.piercing;

    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Bullet> tempBullet = std::make_unique<Bullet>();

        tempBullet->lifeTime = bulletData.lifeTime;
        tempBullet->radius = bulletData.radius;

        tempBullet->mainColor = bulletData.mainColor;

        tempBullet->ogMainColor = bulletData.mainColor;
        
        bullets.push_back(std::move(tempBullet));

        if(!explodes) continue;

        std::unique_ptr<Explosion> tempExplosion = std::make_unique<Explosion>();

        tempExplosion->radius = bulletData.explosionRadius;

        tempExplosion->lifeTime = bulletData.explosionLifeTime;

        explosions.push_back(std::move(tempExplosion));
    }

    for(const auto& bulletptr : this->bullets)
    {
        Bullet* rawBulletPtr = bulletptr.get();

        inactiveBullets.push_back(rawBulletPtr);
    }

    if(!explodes) return;

    for(const auto& explosionptr : this->explosions)
    {
        Explosion* rawExplosionPtr = explosionptr.get();

        inactiveExplosions.push_back(rawExplosionPtr);
    }
}

void BulletPool::UpdateBullets(float dt)
{
    for(int i = 0; i < activeBullets.size();)
    {
        Bullet* b = activeBullets[i];

        b->UpdateBullet(dt);

        b->currentTime += dt;

        if(b->currentTime >= b->lifeTime || b->didHit)
        {
            inactiveBullets.push_back(b);

            activeBullets[i] = activeBullets.back();
            activeBullets.pop_back();

            if(explodes)
            {
                if(!inactiveExplosions.empty())
                {
                    Explosion* e = inactiveExplosions.back();

                    inactiveExplosions.pop_back();

                    e->position = b->posititon;

                    e->currentTime = 0.0f;

                    activeExplosions.push_back(e);
                }
            }
        }
        else
        {
            i++;
        }
    }

    if(!explodes) return;

    for(int i = 0; i < activeExplosions.size();)
    {
        Explosion* e = activeExplosions[i];

        e->currentTime += dt;

        if(e->currentTime >= e->lifeTime)
        {
            inactiveExplosions.push_back(e);

            activeExplosions[i] = activeExplosions.back();
            activeExplosions.pop_back();
        }
        else
        {
            i++;
        }
    }
}

void BulletPool::FireBullet(Vector2 position, Vector2 initialVelocity, float gravity)
{
    if(!inactiveBullets.empty())
    {
        Bullet* b = inactiveBullets.back();
        inactiveBullets.pop_back();

        b->posititon = position;
        b->didHit = false;
        b->currentTime = 0.0f;
        b->velocity = initialVelocity;
        b->gravity = gravity;

        activeBullets.push_back(b);
    }
}
