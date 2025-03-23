#include "textureReserve.h"
#include "renderMesh.h"

#include "surfaceFactory.h"

DEFINE_SINGLETON(RBX::Render::TextureReserve);
using namespace RBX;

Vector2 Render::TextureReserve::calculateSuperTextureDimensions()
{
    float width = 0, height = 0;

    int size = bindedTextures.size();

    for (int i = 0; i < size; i++)
    {
        GImage* t = bindedTextures[i];
        if (!t)
        {
            continue;
        }
        GImage texture = *t;

        if (texture.channels > 0)
        {
            int textureWidth = texture.width, textureHeight = texture.height;

            width += textureWidth; /* everything's gonna be packed together so no check for width */

            if (height < textureHeight) /* prevent stair casing? */
            {
                height += textureHeight;
            }
        }
    }

    return Vector2(width, height);
}

Array<Vector2> Render::TextureReserve::calculateTextureUV(int textureIndex)
{
    Array<Vector2> tuple; /* no tuple class.. and i don't want to make one if it's just gonna be used for this ! */

    TexturePositionalInformation position;
    if (superTexturePositions.containsKey(textureIndex))
    {
        position = superTexturePositions.get(textureIndex);

        Vector2 xy, wh;
        xy = Vector2(position.x, position.y);
        wh = Vector2(position.cx, position.cy);

        tuple.append(xy);
        tuple.append(wh);
    }

    return tuple;
}

void Render::TextureReserve::generateSuperTexture()
{
    Vector2 dimensions = calculateSuperTextureDimensions();

    superTextureData.resize(dimensions.x, dimensions.y, 4);

    int currentX = 0,
        currentY = 0;

    bool dirty = SurfaceFactory::get()->dirty;
    if (dirty)
    {
        SurfaceFactory::get()->dirty = false;
    }

    if (bindedTextures.size() != lastSize || dirty) /* check if textures added / removed */
    {
        for (int i = 0; i < bindedTextures.size(); i++)
        {
            GImage* texture = bindedTextures[i];

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

                if (!superTexturePositions.containsKey(i))
                {
                    TexturePositionalInformation position{};

                    position.x = currentX;
                    position.y = currentY;
                    position.cx = texture->width;
                    position.cy = texture->height;

                    superTexturePositions.set(i, position);
                }

                if (currentX <= superTextureData.width)
                {
                    currentX += texture->width;
                }

            }

        }

        guryWorldTexture = Texture::fromGImage("guryWorldTexture", superTextureData, TextureFormat::AUTO, Texture::Dimension::DIM_2D);

    }

    lastSize = bindedTextures.size();
}