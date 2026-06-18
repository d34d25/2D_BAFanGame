#include "platform.h"
#include <iostream>

void Platform::InitPlatform(
    const Transform2D &transform,
    const EntityData &data,
    const Direction &direction,
    float gravity,
    int textureIndex,
    int variantIndex
)
{
    gameObj.hasBody = true;

    this->gravity = gravity;

    gameObj.transform = transform;
    gameObj.data = data;
    gameObj.direction = direction;

    ogPosition = gameObj.transform.position;

    this->textureIndex = textureIndex;
    this->variantIndex = variantIndex;

    float platformSpeed = 100.0f;

    SetTimerInit(0.3f);

    SetRespawnTimerInit(3.0f);

    gameObj.hitboxes.clear();

    float platformWidth = GRID_SIZE;
    float platformHeight = GRID_SIZE;

    if(type == PlatformType::MOVING_HORIZONTAL ||
    type == PlatformType::MOVING_VERTICAL)
    {
        platformWidth = GRID_SIZE * 3.0f;
        platformHeight = GRID_SIZE * 0.3f;
    }

    Hitbox mainHitbox = {
        {0,0},
        {platformWidth, platformHeight}
    };

    gameObj.hitboxes.push_back(mainHitbox);

    switch (type)
    {
    case PlatformType::MOVING_VERTICAL:
    {
        gameObj.body.velocity.y = -platformSpeed;

        updateRequired = true;
    }
    break;

    case PlatformType::MOVING_HORIZONTAL:
    {
        gameObj.body.velocity.x = platformSpeed;

        updateRequired = true;
    }
    break;

    case PlatformType::FALLING:
    {
        gameObj.body.hasGravity = true;
    }
    break;

    case PlatformType::DISAPPEARING:
    {
        SetRespawnTimerInit(1.0f);
    }
    break;

    case PlatformType::MOVING_SPIKE_VERTICAL:
    {
        platformSpeed *= 1.5f;

        gameObj.body.velocity.y = -platformSpeed;

        updateRequired = true;

        float factor = 0.8f;

        Hitbox subHitbox = {
            {0,0},
            {platformWidth * factor, platformHeight * factor}
        };

        gameObj.hitboxes.push_back(subHitbox);
    }
    break;

    case PlatformType::MOVING_SPIKE_HORIZONTAL:
    {
        platformSpeed *= 1.5f;

        gameObj.body.velocity.x = platformSpeed;

        updateRequired = true;

        float factor = 0.8f;

        Hitbox subHitbox = {
            {0,0},
            {platformWidth * factor, platformHeight * factor}
        };

        gameObj.hitboxes.push_back(subHitbox);
    }
    break;

    case PlatformType::ROTATING_SPIKE_SINGLE:
    {
        updateRequired = true;
        gameObj.body.velocity.x = platformSpeed;

        if(gameObj.data.flipX) gameObj.body.velocity.x = -platformSpeed;

        float size = GRID_SIZE * 0.5f;

        for(int h = 0; h < ROTATING_SPIKE_MAX_HITBOX; h++)
        {
            Vector2 offset = {0,0};

            if(h >= 0)
            {
                float multiplier = (float)h * size;

                offset.x = gameObj.data.flipX ? -multiplier : multiplier;
                offset.y = gameObj.data.flipY ? multiplier : -multiplier;

                Hitbox subHitbox = {
                    offset, {size, size}
                };

                gameObj.hitboxes.push_back(subHitbox);
            }
        }
    }
    break;

    case PlatformType::ROTATING_SPIKE_DOUBLE:
    {
        updateRequired = true;
        gameObj.body.velocity.x = platformSpeed;

        if(gameObj.data.flipX) gameObj.body.velocity.x = -platformSpeed;

        float size = GRID_SIZE * 0.5f;

        for(int h = 0; h < ROTATING_SPIKE_MAX_HITBOX * 2.0f; h++)
        {
            if(h == ROTATING_SPIKE_MAX_HITBOX) continue;

            Vector2 offset = {0,0};

            float armSide = (h < ROTATING_SPIKE_MAX_HITBOX) ? 1.0f : -1.0f;
            int localH = h % ROTATING_SPIKE_MAX_HITBOX;

            if(localH >= 0)
            {
                float multiplier = (float)localH * size * armSide;

                offset.x = gameObj.data.flipX ? -multiplier : multiplier;
                offset.y = gameObj.data.flipY ? multiplier : -multiplier;

                Hitbox subHitbox = {
                    offset, {size, size}
                };

                gameObj.hitboxes.push_back(subHitbox);
            }
        }
    }
    break;

    default: break;
    } 

    renderData = GetPlatformActiveRenderData(type, variantIndex);

    gameObj.UpdateHitboxes();

    Respawn();
}

void Platform::Update(float dt, int iterations)
{
    bool isFalling = type == PlatformType::FALLING;
    bool isDisappearing = type == PlatformType::DISAPPEARING;

    bool isRotatingSpike = type == PlatformType::ROTATING_SPIKE_SINGLE || type == PlatformType::ROTATING_SPIKE_DOUBLE;

    float subDt = dt / iterations;

    if(isFalling || isDisappearing)
    {
        
        if(timer < maxTime)
        {
            timer += subDt;
            return;
        }
        
        if(isDisappearing)
        {
            gameObj.transform.position = DESPAWN_LOCATION;
        }
        else if(isFalling)
        {
            gameObj.body.hasGravity = true;

            gameObj.body.UpdateVelocity(dt, iterations, gravity);

            respawnTimer += subDt;

            if(respawnTimer >= respawnMaxTime)
            {
                gameObj.transform.position = DESPAWN_LOCATION;
            }
        }
    }
    else if(isRotatingSpike)
    {
        rotationAngle = gameObj.body.velocity.x * 0.01f * subDt;

        float cosA = cosf(rotationAngle);
        float sinA = sinf(rotationAngle);

        float size = GRID_SIZE * 0.5f;

        for(int h = 1; h < gameObj.hitboxes.size(); h++)
        {
            Vector2 offset = gameObj.hitboxes[h].offset;

            gameObj.hitboxes[h].offset.x = offset.x * cosA - offset.y * sinA;
            gameObj.hitboxes[h].offset.y = offset.x * sinA + offset.y * cosA;
        }
    }
    else
    {
        gameObj.body.AddVelocities();
    }

    gameObj.body.UpdatePositionX(dt, iterations, &gameObj.transform.position.x);

    gameObj.body.UpdatePositionY(dt, iterations, &gameObj.transform.position.y);

    gameObj.UpdateHitboxes();
}

void Platform::UpdateInactive(float dt, int iterations)
{
    if(!IsInactive()) return;

    float subDt = dt / iterations;

    respawnTimer += subDt;

    if(respawnTimer >= respawnMaxTime)
    {
        timer = 0.0f;
        respawnTimer = 0.0f;

        gameObj.transform.position = ogPosition;

        gameObj.body.hasGravity = false;
        gameObj.body.velocity = {0,0};

        gameObj.UpdateHitboxes();

        updateRequired = false;
    }
}
