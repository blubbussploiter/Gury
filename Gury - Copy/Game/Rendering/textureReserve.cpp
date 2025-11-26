#include "textureReserve.h"
#include "renderMesh.h"

#include "surfaceFactory.h"

#include "brickcolor.h"

#include "../World/workspace.h"

using namespace RBX;

void RBX::Render::TextureReserve::regenWorld()
{
    Instances instances = WorldScene::get()->sceneObjects;

    for (size_t i = 0; i < instances.size(); i++)
    {
        IRenderable* renderable = toInstance<IRenderable>(instances.at(i));
        renderable->regenerateRenderable();
    }
}

RBX::Render::TextureReserve::TexturePositionalInformation RBX::Render::TextureReserve::getSurfaceUV(Color4 brickColor, SurfaceType surface, NormalId normalId, Vector2 sizeTile)
{
    uint32_t brickColor_texture = BrickColor::getColorMap()->getInAtlas(brickColor, surface);

    if (brickColor_texture != -1)
    {
        if (superTexturePositions.containsKey(brickColor_texture))
        {
            TexturePositionalInformation position;
            position = superTexturePositions.get(brickColor_texture);

            if (surface != Smooth)
            {
                position.x += 2;
                position.y += 1;

                position.cx -= 1;
                position.cy -= 1;

                float sx = sizeTile.x;
                float sy = sizeTile.y;

                if (sx != 2 || sy != 4)
                {
                    position.cx = sy * 32;
                    position.cy = sx * 32;
                }

                if (sx > 30 || sy > 30) /* Mega texture, reset */
                {
                    position.cx = 32;
                    position.cy = 64;
                }
            }
            else
            {
                position.x += 5;
                position.y += 5;

                position.cx -= 10;
                position.cy -= 10;
            }

            position.x = position.x / dimensions.x;
            position.y = position.y / dimensions.y;

            position.cx = position.cx / dimensions.x;
            position.cy = position.cy / dimensions.y;

            return position;
        }
    }
    return TexturePositionalInformation();
}

void RBX::Render::TextureReserve::getSurfaceXXYY(Color4 brickColor, SurfaceType surface, NormalId normalId, Vector2 sizeTile, Vector2& u, Vector2& v)
{
    TexturePositionalInformation position = getSurfaceUV(brickColor, surface, normalId, sizeTile);

    u.x = position.x;
    u.y = position.y;

    v.x = position.cx;
    v.y = position.cy;
}

RBX::Render::TextureReserve* RBX::Render::TextureReserve::get()
{
    return Datamodel::get()->textureReserve;
}

Vector2 Render::TextureReserve::calculateSuperTextureDimensions()
{
    int width = 0,
        height = 0;
    int highHeight = 0;

    Array<int> keys = bindedTextures.getKeys();
    Array<int> heights = Array<int>();

    for (int i = 0; i < keys.size(); i++)
    {
        int key = keys[i];
        if (bindedTextures.containsKey(key))
        {
            GImage* t = bindedTextures.get(key);

            if (!t)
            {
                continue;
            }

            GImage texture = *t;

            if (texture.channels > 0)
            {
                width += texture.width; /* everything's gonna be packed together so no check for width */
                heights.push_back(texture.height); /* We'll calculate the height at the end */
            }
        }
    }

    for (int i = 0; i < heights.size(); i++)
    {
        int height = heights[i];
        if (height > highHeight)
        {
            highHeight = height;
        }
    }

    return Vector2(width, highHeight);
}

void Render::TextureReserve::generateSuperTexture()
{

    if (dirty) /* check if textures added / removed */
    {
        StandardOut::print(RBX::MESSAGE_INFO, "Dirty, rebuilding supertexture...");

        dimensions = calculateSuperTextureDimensions();

        dirty = false;
        superTextureData.resize(dimensions.x, dimensions.y, 4);

        int currentX = 0,
            currentY = 0;

        Array<int> keys = bindedTextures.getKeys();

        for (int i = 0; i < keys.size(); i++)
        {
            int key = keys[i];

            GImage* texture = bindedTextures.get(key);

            if (!texture)
            {
                continue;
            }

            if (texture->channels > 0)
            {

                if (!superTextureData.pasteSubImage(superTextureData, *texture, currentX, currentY, 0, 0, texture->width, texture->height))
                {
                    StandardOut::print(MESSAGE_WARNING, "could not paste sub image at %d! ignoring", i);
                    continue;
                }

                TexturePositionalInformation position{};

                position.x = currentX;
                position.y = currentY;
                position.cx = texture->width;
                position.cy = texture->height;

                superTexturePositions.set(key, position);

                if (currentX <= superTextureData.width)
                {
                    currentX += texture->width;
                }

            }

        }

        guryWorldTexture = Texture::fromGImage("guryWorldTexture", superTextureData, TextureFormat::AUTO);

        superTextureData.save("D:\\gury test stuff\\gurySuperImage.png");

        if (!WorldManager::get()->dirty)
        {
            regenWorld();
        }
    }

}