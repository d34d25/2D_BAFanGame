#pragma once

#include "raylib.h"
#include "entity.h"

inline void DrawAABB(Rectangle aabb, Color color)
{
    float thickness = 1.0f;
    //up-left to up-right
    DrawLineEx({aabb.x, aabb.y}, {aabb.x + aabb.width, aabb.y}, thickness, color);
    
    //up-left to down-left
    DrawLineEx({aabb.x, aabb.y}, {aabb.x, aabb.y + aabb.height}, thickness, color);

    //up-right to down-right
    DrawLineEx({aabb.x + aabb.width, aabb.y}, {aabb.x + aabb.width, aabb.y + aabb.height}, thickness, color);

    //down-left to down-right
    DrawLineEx({aabb.x, aabb.y + aabb.height}, {aabb.x + aabb.width, aabb.y + aabb.height}, thickness, color);
}

inline std::vector<Rectangle> CropImage(const Texture2D& sourceTexture, Vector2 size)
{
    std::vector<Rectangle> frames = {};

    if(sourceTexture.id <= 0 || size.x == 0 || size.y == 0) return frames;

    int gap = 1;

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

inline int GetCurrentFrame(const std::vector<Rectangle>& frames, int index, int spacing, float animationSpeed)
{
    if(frames.empty()) return 0;

    int frameCount = (spacing <= 0) ? (int)frames.size() : spacing;

    int frame = (int)(GetTime() * animationSpeed) % frameCount;

    return index + frame;
}

inline void DrawSprite(
    const Transform2D& transform,
    const SpriteRenderData& renderData,
    const EntityData& entityData,
    int currentFrame = 0
)
{

    Rectangle sourceRect = Rectangle{0,0, (float)renderData.sourceTexture->width, (float)renderData.sourceTexture->height};

    if(!renderData.animationFrames.empty() && currentFrame < renderData.animationFrames.size())
    {
        sourceRect = renderData.animationFrames[currentFrame];
    }

    if(entityData.flipX) sourceRect.width = -sourceRect.width;
    if(entityData.flipY) sourceRect.height = -sourceRect.height;

    float width = fabs(sourceRect.width) * transform.scale;
    float height = fabs(sourceRect.height) * transform.scale;

    float offsetX = entityData.flipX ? -renderData.offset.x : renderData.offset.x;
    float offsetY = entityData.flipY ? -renderData.offset.y : renderData.offset.y;

    Rectangle destRect = {
        roundf(transform.position.x + offsetX),
        roundf(transform.position.y + offsetY),
        width,
        height
    };

    Vector2 origin = {width * 0.5f, height * 0.5f};

    DrawTexturePro(
        *renderData.sourceTexture,
        sourceRect,
        destRect,
        origin,
        0.0f,
        WHITE
    );
}

inline void DrawTile(SpriteRenderData* renderData, int frameIndex, const Transform2D& transform, Color color = WHITE)
{
    if(!renderData || frameIndex < 0 || frameIndex >= renderData->animationFrames.size())
        return;

    Rectangle source = renderData->animationFrames[frameIndex];

    float width = source.width * transform.scale;
    float height = source.height * transform.scale;

    Rectangle dest = {transform.position.x, transform.position.y, width, height};

    Vector2 origin = {width * 0.5f, height * 0.5f};

    DrawTexturePro(
        *renderData->sourceTexture,
        source,
        dest,
        origin, 
        0.0f,
        color
    );
}

inline void DrawBullet(int x, int y, float radius, Color mainColor, Color backColor)
{
    if(!ColorIsEqual(backColor, BLACK)) DrawCircleGradient({(float)x,(float)y}, radius * 1.2f, mainColor, backColor);
    else DrawCircle(x, y, radius * 1.2f, backColor);

    DrawCircle(x, y, radius, mainColor);
}

inline void DrawExplosion(int x, int y, float radius, SpriteRenderData* renderData, int frameIndex = 0, float scale = tileScale, Color color = WHITE)
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