#pragma once

#include "raylib.h"
#include "entity.h"

inline void DrawSprite(
    const SpriteRenderData& renderData,
    const EntityData& entityData
)
{
    float scale = renderData.scale;
    Vector2 position = renderData.position;

    float width = fabs(renderData.sourceRect.width) * scale;
    float height = fabs(renderData.sourceRect.height) * scale;

    Rectangle destRect = {
        roundf(position.x),
        roundf(position.y),
        width,
        height
    };

    Vector2 origin = {width * 0.5f, height * 0.5f};

    Rectangle sourceRect = renderData.sourceRect;

    if(entityData.flipX) sourceRect.width = -sourceRect.width;

    if(entityData.flipY) sourceRect.height = -sourceRect.height;

    DrawTexturePro(
        renderData.texture,
        sourceRect,
        destRect,
        origin,
        0.0f,
        WHITE
    );
}

inline void DrawAABB(Rectangle aabb, Color color)
{
    DrawRectangleLines(aabb.x, aabb.y, aabb.width, aabb.height, color);
}

inline void DrawSpike()
{
    
}