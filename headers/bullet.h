#pragma once

#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <memory>

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

class Bullet
{

public:

    Vector2 posititon = {0,0};

    Vector2 velocity = {0,0};

    bool didHit = false;

    float radius = 2.0f;

    float lifeTime = 0;
    float currentTime = 0;

    float gravity = 0;

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

    BulletPool() = default;

    BulletPool(int quantity, float lifeTime, float radius, Color mainColor, Color backColor);

    ~BulletPool() = default;

    void UpdateBullets(float dt);

    void FireBullet(Vector2 position, Vector2 initialVelocity, float gravity);
};

inline float GenerateBulletSpread(float angle, float spread)
{
    float random = (float)GetRandomValue(-1000,1000) / 1000.0f;

    return angle + (random * spread);
}