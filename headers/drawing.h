#pragma once

#include "raylib.h"
#include "entity.h"

inline void DrawAABB(Rectangle aabb, Color color, float thickness = 1.0f)
{
    //up-left to up-right
    DrawLineEx({aabb.x, aabb.y}, {aabb.x + aabb.width, aabb.y}, thickness, color);
    
    //up-left to down-left
    DrawLineEx({aabb.x, aabb.y}, {aabb.x, aabb.y + aabb.height}, thickness, color);

    //up-right to down-right
    DrawLineEx({aabb.x + aabb.width, aabb.y}, {aabb.x + aabb.width, aabb.y + aabb.height}, thickness, color);

    //down-left to down-right
    DrawLineEx({aabb.x, aabb.y + aabb.height}, {aabb.x + aabb.width, aabb.y + aabb.height}, thickness, color);
}

inline void DrawAABB(float x, float y, float width, float height, Color color, float thickness = 1.0f)
{
    //up-left to up-right
    DrawLineEx({x, y}, {x + width, y}, thickness, color);
    
    //up-left to down-left
    DrawLineEx({x, y}, {x, y + height}, thickness, color);

    //up-right to down-right
    DrawLineEx({x + width, y}, {x + width, y + height}, thickness, color);

    //down-left to down-right
    DrawLineEx({x, y + height}, {x + width, y + height}, thickness, color);
}

inline std::vector<Rectangle> CropImage(const Texture2D& sourceTexture, Vector2 size, int gap = 0)
{
    std::vector<Rectangle> frames = {};

    if(sourceTexture.id <= 0 || size.x == 0 || size.y == 0) return frames;

    int framesPerRow = sourceTexture.width / ((int)size.x + gap);
    int framesPerCol = sourceTexture.height / ((int)size.y + gap);

    int totalFrames = framesPerRow * framesPerCol;

    for(int i = 0; i < totalFrames; i++)
    {
        float x = (float)(i % framesPerRow) * (size.x + gap) + gap;
        float y = (float)(i / framesPerRow) * (size.y + gap) + gap;

        frames.push_back(Rectangle{x, y, size.x, size.y});
    }

    return frames;
}

inline int GetCurrentFrame(const std::vector<Rectangle>& frames, const int index, int spacing, float animationSpeed, double currentTime)
{
    if(frames.empty()) return 0;

    int frameCount = (spacing <= 0) ? (int)frames.size() : spacing;

    int frame = (int)(currentTime * animationSpeed) % frameCount;

    return index + frame;
}

inline void DrawSprite(
    const Transform2D& transform,
    const SpriteRenderData* renderData,
    const EntityData& entityData,
    int currentFrame = 0,
    Color color = WHITE
)
{

    if(!renderData || currentFrame < 0 || currentFrame >= renderData->animationFrames.size())
        return;

    Rectangle sourceRect = renderData->animationFrames[currentFrame];

    switch ((int)transform.angle)
    {
    case 0:
    case 180:
    {
        if(entityData.flipX) sourceRect.width = -sourceRect.width;
        if(entityData.flipY) sourceRect.height = -sourceRect.height;
    }
    break;

    case 90:
    case 270:
    {
        if(entityData.flipY) sourceRect.width = -sourceRect.width;
        if(entityData.flipX) sourceRect.height = -sourceRect.height;
    }
    break;
    
    default:
        break;
    }

    float width = fabs(sourceRect.width) * transform.scale;
    float height = fabs(sourceRect.height) * transform.scale;

    float offsetX = entityData.flipX ? -renderData->offset.x : renderData->offset.x;
    float offsetY = entityData.flipY ? -renderData->offset.y : renderData->offset.y;

    Rectangle destRect = {
        roundf(transform.position.x + offsetX),
        roundf(transform.position.y + offsetY),
        width,
        height
    };

    Vector2 origin = {width * 0.5f, height * 0.5f};

    DrawTexturePro(
        *renderData->sourceTexture,
        sourceRect,
        destRect,
        origin,
        transform.angle,
        color
    );
}

inline void DrawSprite(GameObject& gameObj, SpriteRenderData* renderData, int currentFrame = 0, Color color = WHITE)
{
    DrawSprite(
        gameObj.transform,
        renderData,
        gameObj.data,
        currentFrame,
        color
    );
};

inline void DrawBullet(int x, int y, float radius, Color mainColor, Color backColor)
{
    if(!ColorIsEqual(backColor, BLACK)) DrawCircleGradient({(float)x,(float)y}, radius * 1.2f, mainColor, backColor);
    else DrawCircle(x, y, radius * 1.2f, backColor);

    DrawCircle(x, y, radius, mainColor);
};

inline void DrawExplosion(int x, int y, float radius, SpriteRenderData* renderData, int frameIndex = 0, float scale = TILE_SCALE, Color color = WHITE)
{
    if(renderData)
    {
        //WIP
    }
    else
    {
        DrawCircleLines(x, y, radius, RED);
    }
}