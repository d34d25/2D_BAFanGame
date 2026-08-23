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
        inactiveBullets.push_back(bulletptr.get());
    }

    if(!explodes) return;

    for(const auto& explosionptr : this->explosions)
    {
        inactiveExplosions.push_back(explosionptr.get());
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

EffectPool::EffectPool(int quantity, float lifeTime, bool wobbles)
{
    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Effect> tempEffect = std::make_unique<Effect>();

        tempEffect->lifeTime = lifeTime;
        
        tempEffect->wobbles = wobbles;

        effects.push_back(std::move(tempEffect));
    }

    for(const auto& effectPtr : this->effects)
    {
        inactiveEffects.push_back(effectPtr.get());
    }
}

void EffectPool::UpdateEffects(float dt)
{
    for(int i = 0; i < activeEffects.size();)
    {
        Effect* e = activeEffects[i];

        e->UpdateEffect(dt);

        e->currentTime += dt;

        if(e->currentTime >= e->lifeTime || e->radius >= MAX_RADIUS)
        {
            inactiveEffects.push_back(e);

            activeEffects[i] = activeEffects.back();

            activeEffects.pop_back();
        }
        else
        {
            i++;
        }
    }
}

void EffectPool::SpawnEffect(Vector2 position, Vector2 initialVelocity)
{
    if(!inactiveEffects.empty())
    {
        Effect* e = inactiveEffects.back();

        inactiveEffects.pop_back();

        e->position = position;
        e->currentTime = 0.0f;
        e->velocity = initialVelocity;
        
        e->radius = e->ogRadius;

        activeEffects.push_back(e);
    }
}
