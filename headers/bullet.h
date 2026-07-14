#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

#include "entity.h"

struct BulletProperties
{
    Color mainColor = RAYWHITE;
    Color backColor = BLACK;

    float fireTimer = 0;
    float fireRate = 1.0f;
    float lifeTime = 2.0f;
    float gravity = 0;
    float speed = 0;
    float angle = 0;
    float spread = 0;
    float radius = 2.0f;

    float explosionRadius = 17.0f;
    float explosionLifeTime = 0.5f;

    int pelletCount = 1;

    bool explodes = false;

    bool piercing = false;
};

struct Explosion
{
    Vector2 position = {0.0f,0.0f};

    SpriteRenderData* renderData = nullptr;

    float radius = 10.0f;

    float lifeTime = 1.0f;

    float currentTime = 0.0f;
};

class Bullet
{

public:

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

class BulletPool
{
public:

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
        int quantity, const BulletProperties& bulletData,
        SpriteRenderData* explosionRenderData = nullptr
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
            float radians = GenerateBulletSpread(angle, bulletData.spread) * (PI / 180.0f);

            Vector2 initialVel = {0,0};

            initialVel.x = bulletData.speed * cosf(radians) + gameObj.body.velocity.x;
            initialVel.y = bulletData.speed * sinf(radians);

            bulletpool->FireBullet(bulletSpawnPos, initialVel, bulletGravity);
        }

        bulletData.fireTimer = bulletData.fireRate;
    }
}