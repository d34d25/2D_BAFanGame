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

inline std::vector<Rectangle> CropImage(const Texture2D& sourceTexture, int maxFrames, Vector2 size)
{
    std::vector<Rectangle> frames = {};

    if(sourceTexture.id <= 0 || size.x == 0 || size.y == 0) return frames;

    int gap = 1;

    int framesPerRow = sourceTexture.width / ((int)size.x + gap);

    for(int i = 0; i < maxFrames; i++)
    {
        float x = (float)(i % framesPerRow) * (size.x + gap) + gap;
        float y = (float)(i / framesPerRow) * (size.y + gap) + gap;

        frames.push_back(Rectangle{x, y, size.x, size.y});
    }

    return frames;
}

inline int GetCurrentFrame(const std::vector<Rectangle>& frames, int startFrame, int endFrame, float animationSpeed)
{
    if(frames.empty()) return 0;

    if(startFrame < 0) startFrame = 0;

    if(endFrame >= (int)frames.size()) endFrame = (int)frames.size() - 1;

    if(startFrame > endFrame) return startFrame;

    int frameCount = (endFrame - startFrame) + 1;

    int currentLoopFrame = (int)(GetTime() * animationSpeed) % frameCount;

    return startFrame + currentLoopFrame;
}

inline void DrawSprite(
    const Vector2& position,
    const SpriteRenderData& renderData,
    const EntityData& entityData,
    float scale,
    int currentFrame
)
{

    Rectangle sourceRect = Rectangle{0,0, (float)renderData.sourceTexture.width, (float)renderData.sourceTexture.height};

    if(!renderData.animationFrames.empty() && currentFrame < renderData.animationFrames.size())
    {
        sourceRect = renderData.animationFrames[currentFrame];
    }

    if(entityData.flipX) sourceRect.width = -sourceRect.width;
    if(entityData.flipY) sourceRect.height = -sourceRect.height;

    float width = fabs(sourceRect.width) * scale;
    float height = fabs(sourceRect.height) * scale;

    float offsetX = entityData.flipX ? -renderData.offset.x : renderData.offset.x;
    float offsetY = entityData.flipY ? -renderData.offset.y : renderData.offset.y;

    Rectangle destRect = {
        roundf(position.x + offsetX),
        roundf(position.y + offsetY),
        width,
        height
    };

    Vector2 origin = {width * 0.5f, height * 0.5f};

    DrawTexturePro(
        renderData.sourceTexture,
        sourceRect,
        destRect,
        origin,
        0.0f,
        WHITE
    );
}

inline void DrawTile(SpriteRenderData* renderData, int frameIndex, Vector2 worldPos, float scale, Color color = WHITE)
{
    if(!renderData || frameIndex < 0 || frameIndex >= renderData->animationFrames.size())
        return;

    Rectangle source = renderData->animationFrames[frameIndex];

    float width = source.width * scale;
    float height = source.height * scale;

    Rectangle dest = {worldPos.x, worldPos.y, width, height};

    Vector2 origin = {width * 0.5f, height * 0.5f};

    DrawTexturePro(
        renderData->sourceTexture,
        source,
        dest,
        origin, 
        0.0f,
        color
    );
}