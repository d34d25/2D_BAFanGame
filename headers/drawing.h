#pragma once

#include "raylib.h"
#include "entity.h"

#include "array"

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

    int rowStart = (index / frameCount) * frameCount;

    int ogColumn = index % frameCount;

    int framesAdvanced = (int)(currentTime * animationSpeed);

    int animatedCol = (ogColumn + framesAdvanced) % frameCount;

    int frame = rowStart + animatedCol;

    return frame % (int)frames.size();
}

inline void DrawTextureScaled(int x, int y, const Texture2D& texture, Color tint = WHITE)
{
    float width = fabs(texture.width);
    float height = fabs(texture.height);

    Rectangle destRect = {
        (float)x,
        (float)y,
        width,
        height
    };

    Vector2 origin = {floorf(width * 0.5f), floorf(height * 0.5f)};

    DrawTexturePro(
        texture,
        Rectangle{0,0,(float)texture.width, (float)texture.height},
        destRect,
        origin,
        0,
        tint
    );
}

inline void DrawSprite(
    const SpriteRenderData* renderData,
    float x, float y,
    int frame,
    bool flipX = false, bool flipY = false,
    Color tint = WHITE
)
{
    if(!renderData || frame < 0 || frame >= renderData->animationFrames.size())
        return;

    Rectangle sourceRect = renderData->animationFrames[frame];

    float width = fabs(sourceRect.width);
    float height = fabs(sourceRect.height);

    float offsetX = flipX ? -renderData->offset.x : renderData->offset.x;
    float offsetY = flipY ? -renderData->offset.y : renderData->offset.y;

    Rectangle destRect = {
        floorf(x + offsetX),
        floorf(y + offsetY),
        width,
        height
    };

    Vector2 origin = {floorf(width * 0.5f), floorf(height * 0.5f)};

    DrawTexturePro(
        *renderData->sourceTexture,
        sourceRect,
        destRect,
        origin,
        0,
        tint
    );
}

inline void DrawSprite(
    Vector2 position,
    float angle,
    const SpriteRenderData* renderData,
    const SpriteFlipData& entityData,
    int currentFrame = 0,
    Color color = WHITE
)
{
    if(!renderData || currentFrame < 0 || currentFrame >= renderData->animationFrames.size())
        return;

    Rectangle sourceRect = renderData->animationFrames[currentFrame];

    switch ((int)angle)
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
        if(entityData.flipX) sourceRect.width = -sourceRect.width;
        if(entityData.flipY) sourceRect.height = -sourceRect.height;
    }
    break;
    
    default:
        break;
    }

    float width = fabs(sourceRect.width);
    float height = fabs(sourceRect.height);

    float offsetX = entityData.flipX ? -renderData->offset.x : renderData->offset.x;
    float offsetY = entityData.flipY ? -renderData->offset.y : renderData->offset.y;

    float xpos = floorf(position.x + renderData->offset.x);
    float ypos = floorf(position.y + renderData->offset.y);

    if(entityData.flipOffset)
    {
        xpos = floorf(position.x + offsetX);
        ypos = floorf(position.y + offsetY);
    };

    Rectangle destRect = {
        xpos,
        ypos,
        width,
        height
    };

    Vector2 origin = {floorf(width * 0.5f), floorf(height * 0.5f)};

    DrawTexturePro(
        *renderData->sourceTexture,
        sourceRect,
        destRect,
        origin,
        angle,
        color
    );

}

inline void DrawSprite(
    const Transform2D& transform,
    const SpriteRenderData* renderData,
    const SpriteFlipData& entityData,
    int currentFrame = 0,
    Color color = WHITE
)
{
    DrawSprite(
        transform.position,
        transform.angle,
        renderData,
        entityData,
        currentFrame,
        color
    );
}

inline void DrawSprite(const GameObject& gameObj, SpriteRenderData* renderData, int currentFrame = 0, Color color = WHITE)
{
    DrawSprite(
        gameObj.transform,
        renderData,
        gameObj.flipData,
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

inline void DrawExplosion(int x, int y, float radius, SpriteRenderData* renderData, int frameIndex = 0, Color color = WHITE)
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

inline void SetShaderPalette(Shader shader, int location, const std::array<Color, 4>& palette)
{
    float floats[16];

    for(size_t i = 0; i < palette.size(); i++)
    {
        floats[i * MAX_PALETTE_COLS + 0] = (float)palette[i].r / 255.0f;
        floats[i * MAX_PALETTE_COLS + 1] = (float)palette[i].g / 255.0f;
        floats[i * MAX_PALETTE_COLS + 2] = (float)palette[i].b / 255.0f;
        floats[i * MAX_PALETTE_COLS + 3] = (float)palette[i].a / 255.0f;
    }

    SetShaderValueV(shader, location, floats, SHADER_UNIFORM_VEC4, 4);
}