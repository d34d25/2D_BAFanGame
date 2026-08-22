#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

#include "entity.h"

struct BulletProperties
{
    Color mainColor = BULLET_COLOR;

    float fireTimer = 0;
    float fireRate = 1.0f;
    float lifeTime = 2.0f;
    float gravity = 0;
    float speed = 0;
    float angle = 0;
    float spread = 0;
    float radius = 2.0f;

    float explosionRadius = 12;
    float explosionLifeTime = 0.5f;

    int pelletCount = 1;

    bool explodes = false;

    bool piercing = false;

    bool fanShaped = false;

    bool inertia = true;
};

struct Explosion
{
    Vector2 position = {0.0f,0.0f};

    float radius = 10.0f;

    float lifeTime = 1.0f;

    float currentTime = 0.0f;
};

struct Bullet
{
    Vector2 posititon = {0,0};

    Vector2 velocity = {0,0};

    bool didHit = false;

    float radius = 2.0f;

    float lifeTime = 0.0f;
    float currentTime = 0.0f;

    float gravity = 0.0f;

    Color mainColor = RAYWHITE;

    Color backColor = BLACK;

    Color ogMainColor = RAYWHITE;

    Bullet() = default;

    ~Bullet() = default;

    inline void UpdateBullet(float dt)
    {
        velocity.y += gravity * dt;

        posititon += velocity * dt;
    }
};

struct BulletPool
{
    std::vector<std::unique_ptr<Bullet>> bullets = {};
    std::vector<std::unique_ptr<Explosion>> explosions = {};

    std::vector<Bullet*> activeBullets = {};
    std::vector<Bullet*> inactiveBullets = {};
   
    std::vector<Explosion*> activeExplosions = {};
    std::vector<Explosion*> inactiveExplosions = {};

    bool explodes = false;
    bool pierces = false;

    BulletPool() = default;

    BulletPool(
        int quantity, const BulletProperties& bulletData
    );

    ~BulletPool() = default;

    void UpdateBullets(float dt);

    void FireBullet(Vector2 position, Vector2 initialVelocity, float gravity);

    inline void Reset()
    {
        for(Bullet* b : activeBullets)
        {
            inactiveBullets.push_back(b);
        }

        activeBullets.clear();

        for(Explosion* e : activeExplosions)
        {
            inactiveExplosions.push_back(e);
        }

        activeExplosions.clear();
    }
};

inline float GenerateBulletSpread(float angle, float spread)
{
    float random = (float)GetRandomValue(-1000,1000) / 1000.0f;

    return angle + (random * spread);
}

inline void ShootBullet(
    float dt,
    const GameObject& gameObj,
    BulletProperties& bulletData,
    const Vector2& bulletSpawnPos,
    BulletPool* bulletpool,
    bool condition
)
{
    
    if(bulletData.fireTimer > 0.0f) bulletData.fireTimer -= dt;

    if(condition && bulletData.fireTimer <= 0.0f)
    {
        float angle = bulletData.angle;

        float bulletGravity = bulletData.gravity;

        if(gameObj.flipData.flipX)
        {
            angle = 180.0f - angle;
        }

        if(gameObj.flipData.flipY)
        {
            angle = -angle;
            bulletGravity = -bulletData.gravity;
        }

        for(int i = 0; i < bulletData.pelletCount; i++)
        {
            float radians = angle;

            if(bulletData.fanShaped)
            {
                int step = i % bulletData.pelletCount;

                if(gameObj.flipData.flipY)
                {
                    if(gameObj.flipData.flipX) radians = angle + bulletData.spread * step;
                    else radians = angle - bulletData.spread * step;
                }
                else if(gameObj.flipData.flipX) radians = angle - bulletData.spread * step;
                else radians = angle + bulletData.spread * step;
            }
            else
            {
                radians = GenerateBulletSpread(angle, bulletData.spread);
            }

            radians *= (PI / 180.0f);

            Vector2 initialVel = {0,0};

            if(bulletData.inertia)
            {
                initialVel.x = bulletData.speed * cosf(radians) + gameObj.body.velocity.x;
            }
            else
            {
                initialVel.x = bulletData.speed * cosf(radians);
            }

            initialVel.y = bulletData.speed * sinf(radians);

            bulletpool->FireBullet(bulletSpawnPos, initialVel, bulletGravity);
        }

        bulletData.fireTimer = bulletData.fireRate;
    }
}

//Effects

struct Effect
{
    Vector2 position = {0,0};

    Vector2 velocity = {0,0};

    float lifeTime = 0.0f;
    float currentTime = 0.0f;

    float radius = 4.0f;

    Color color = BULLET_COLOR;

    bool wobbles = false;

    Effect() = default;

    ~Effect() = default;

    inline void UpdateEffect(float dt)
    {
        //if(wobbles);

        position += velocity * dt;
    }
};

struct EffectPool
{
    std::vector<std::unique_ptr<Effect>> effects = {};

    std::vector<Effect*> activeEffects = {};
    std::vector<Effect*> inactiveEffects = {};

    EffectPool() = default;

    EffectPool(int quantity);

    ~EffectPool() = default;

    void UpdateEffects(float dt);

    void SpawnEffect(Vector2 position, Vector2 initialVelocity);

    inline void Reset()
    {
        for(Effect* e : activeEffects)
        {
            inactiveEffects.push_back(e);
        }

        activeEffects.clear();
    }
};

inline void ShootEffect(
    float dt,
    Vector2 spawnPos,
    EffectPool* effectPool,
    float speed,
    int pelletCount = 1
)
{
    float spread = 45;

    for(int i = 0; i < pelletCount; i++)
    {
        float radians = 0;

        int step = i % pelletCount;

        radians += spread * step;

        radians *= (PI / 180.0f);

        Vector2 initialVel = {0,0};

        initialVel.x = speed * cosf(radians);

        initialVel.y = speed * sinf(radians);

        effectPool->SpawnEffect(spawnPos, initialVel);
    }
};