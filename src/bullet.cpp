#include "bullet.h"

BulletPool::BulletPool(int quantity, float lifeTime, float radius, Color mainColor, Color backColor)
{
    for(int i = 0; i < quantity; i++)
    {
        std::unique_ptr<Bullet> tempBullet = std::make_unique<Bullet>();

        tempBullet->didHit = false;

        tempBullet->posititon = {0,0};
        tempBullet->velocity = {0,0};

        tempBullet->currentTime = 0;
        tempBullet->gravity = 0;

        tempBullet->lifeTime = lifeTime;
        tempBullet->radius = radius;

        tempBullet->mainColor = mainColor;
        tempBullet->backColor = backColor;
        
        bullets.push_back(std::move(tempBullet));
    }

    for(const auto& bulletptr : this->bullets)
    {
        Bullet* rawBulletPtr = bulletptr.get();

        inactiveBullets.push_back(rawBulletPtr);
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
