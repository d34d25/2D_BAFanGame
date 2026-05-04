#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

#include "entity.h"

struct BulletProperties
{
    float fireTimer = 0;
    float fireRate = 1.0f;
    float gravity = 0;
    float speed = 0;
    float angle = 0;
    float spread = 0;
    float radius = 2.0f;
    Color mainColor = RAYWHITE;
    Color backColor = BLACK;
};

struct Explosion
{
    SpriteRenderData* renderData = nullptr;

    Vector2 position = {0.0f,0.0f};

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

    std::vector<Bullet*> activeBullets = {};
    std::vector<Bullet*> inactiveBullets = {};

    std::vector<std::unique_ptr<Bullet>> bullets = {};

    bool explodes = false;

    std::vector<Explosion*> activeExplosions = {};
    std::vector<Explosion*> inactiveExplosions = {};

    std::vector<std::unique_ptr<Explosion>> explosions = {};

    BulletPool() = default;

    BulletPool(
        int quantity, float lifeTime, float radius, Color mainColor, Color backColor, 
        bool explodes = false, float explosionRadius = 70.0f, float explosionLifeTime = 0.5f, SpriteRenderData* explosionRenderData = nullptr
    );

    ~BulletPool() = default;

    void UpdateBullets(float dt);

    void FireBullet(Vector2 position, Vector2 initialVelocity, float gravity);
};

inline float GenerateBulletSpread(float angle, float spread)
{
    float random = (float)GetRandomValue(-1000,1000) / 1000.0f;

    return angle + (random * spread);
}