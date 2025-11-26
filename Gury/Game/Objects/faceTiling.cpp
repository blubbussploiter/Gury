
#include "../Objects/PVInstance/pvenums.h"
#include "part.h"

using namespace RBX;

void RBX::PartInstance::getSliceInformation(NormalId face, SurfaceType surface, Vector3 realSize, Vector2& sliceSize, Vector2& u, Vector2& v)
{
    sliceSize = PartInstance::getSubdivisionNumbers(face, realSize);
    Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sliceSize, u, v);
}

void PartInstance::appendTexCoordsXYWH(Array<Vector2>& texCoordsOut, Vector2 ru, Vector2 rv, Vector2 size, float width, float height)
{
    bool writeHorizontal = false;

    if (size.x > size.y)
    {
        writeHorizontal = true;
    }
    else if (size.x == size.y)
    {
        writeHorizontal = true;
    }
    else
    {
        writeHorizontal = false;
        if (size.x <= 1)
        {
            writeHorizontal = true;
        }
    }

    appendHelpedTexCoords(texCoordsOut, ru, rv, size, width, height, writeHorizontal);
}

void RBX::PartInstance::appendHelpedTexCoords(Array<Vector2>& texCoordsOut, Vector2 ru, Vector2 rv, Vector2 size, float w, float h, bool drawHorizontal)
{

    if (w >= 30 || h >= 30)
    {
        Render::TextureReserve* reserve = Render::TextureReserve::get();
        Vector2 imageDimensions = reserve->getSuperImageDimensions();

        rv.y = (32 * w) / imageDimensions.y;
        if (rv.y > 4)
        {
            rv.y = floor(rv.y);
        }
        ru.y = -rv.y;

        rv.x = (32 * size.x) / imageDimensions.x;

        drawHorizontal = false;
    }

    if (drawHorizontal)
    {
        texCoordsOut.push_back(Vector2(ru.x, ru.y + rv.y));
        texCoordsOut.push_back(ru);
        texCoordsOut.push_back(Vector2(ru.x + rv.x, ru.y));

        texCoordsOut.push_back(Vector2(ru.x, ru.y + rv.y));
        texCoordsOut.push_back(Vector2(ru.x + rv.x, ru.y));
        texCoordsOut.push_back(Vector2(ru + rv));

    }
    else
    {
        texCoordsOut.push_back(ru);
        texCoordsOut.push_back(Vector2(ru.x + rv.x, ru.y));
        texCoordsOut.push_back(ru + rv);

        texCoordsOut.push_back(ru);
        texCoordsOut.push_back(ru + rv);
        texCoordsOut.push_back(Vector2(ru.x, ru.y + rv.y));

    }
}

void RBX::PartInstance::rbxAppendProductSlice(NormalId face, SurfaceType surface, Vector2 sliceSize, Vector2 u, Vector2 v, float cx, float cy, float xw, float yh, float worldY, float width, float height, Array<Vector3>& out, Array<Vector2>& texCoordsOut)
{
    switch (face)
    {
    case Top:
    {
        out.append(Vector3(xw, worldY, yh));
        out.append(Vector3(xw, worldY, cy));
        out.append(Vector3(cx, worldY, cy));

        out.append(Vector3(xw, worldY, yh));
        out.append(Vector3(cx, worldY, cy));
        out.append(Vector3(cx, worldY, yh));

        appendTexCoordsXYWH(texCoordsOut, u, v, sliceSize, width, height);

        break;
    }
    case Left:
    {
        out.append(Vector3(size.x, xw, yh));
        out.append(Vector3(size.x, cx, yh));
        out.append(Vector3(size.x, cx, cy));

        out.append(Vector3(size.x, xw, yh));
        out.append(Vector3(size.x, cx, cy));
        out.append(Vector3(size.x, xw, cy));

        appendTexCoordsXYWH(texCoordsOut, u, v, sliceSize, width, height);
        break;
    }
    case Bottom:
    {

        out.append(Vector3(xw, -worldY, cy));
        out.append(Vector3(xw, -worldY, yh));
        out.append(Vector3(cx, -worldY, yh));

        out.append(Vector3(xw, -worldY, cy));
        out.append(Vector3(cx, -worldY, yh));
        out.append(Vector3(cx, -worldY, cy));

        appendTexCoordsXYWH(texCoordsOut, u, v, sliceSize, width, height);

        break;
    }
    }
}

void RBX::PartInstance::rbxAppendRemainderSlice(NormalId face, SurfaceType surface, Vector2 sliceSize, float rn0, float rn1, float rn2, float x, float y, float cx, float cy, float xw, float yh, float worldX, float worldY, float sliceW, float sliceH, float width, float height, Array<Vector3>& out, Array<Vector2>& texCoordsOut)
{

    switch (face)
    {
    case Top:
    {
        if (!rn2)
        {
            if (rn0 > 0)
            {
                out.push_back(Vector3(xw, worldY, yh));
                out.push_back(Vector3(xw, worldY, y));
                out.push_back(Vector3(cx, worldY, y));

                out.push_back(Vector3(xw, worldY, yh));
                out.push_back(Vector3(cx, worldY, y));
                out.push_back(Vector3(cx, worldY, yh));

                break;
            }
            else if (rn1 > 0)
            {
                out.push_back(Vector3(xw, worldY, yh)); /* 1 */
                out.push_back(Vector3(xw, worldY, cy)); /* 2 */
                out.push_back(Vector3(x, worldY, cy)); /* 3 */

                out.push_back(Vector3(xw, worldY, yh)); /* 1*/
                out.push_back(Vector3(x, worldY, cy)); /* 3 */
                out.push_back(Vector3(x, worldY, yh)); /* 0 */

                break;
            }
        }
        else
        {
            out.push_back(Vector3(xw, worldY, yh));
            out.push_back(Vector3(xw, worldY, cy));
            out.push_back(Vector3(cx, worldY, cy));

            out.push_back(Vector3(xw, worldY, yh));
            out.push_back(Vector3(cx, worldY, cy));
            out.push_back(Vector3(cx, worldY, yh));
            break;
        }
        break;
    }
    case Bottom:
    {
        if (!rn2)
        {
            /* add remainders */
            if (rn0 > 0)
            {
                out.append(Vector3(xw, -worldY, y));/* 1*/
                out.append(Vector3(xw, -worldY, yh));/* 2*/
                out.append(Vector3(cx, -worldY, yh)); /* 3*/

                out.append(Vector3(xw, -worldY, y)); /* 1*/
                out.append(Vector3(cx, -worldY, yh)); /* 3*/
                out.append(Vector3(cx, -worldY, y)); /* 0*/

                break;
            }
            if (rn1 > 0)
            {
                out.append(Vector3(xw, -worldY, cy));
                out.append(Vector3(xw, -worldY, yh));
                out.append(Vector3(x, -worldY, yh));

                out.append(Vector3(xw, -worldY, cy));
                out.append(Vector3(x, -worldY, yh));
                out.append(Vector3(x, -worldY, cy));

                break;
            }
        }
        else
        {

            out.append(Vector3(xw, -worldY, cy));
            out.append(Vector3(xw, -worldY, yh));
            out.append(Vector3(cx, -worldY, yh));

            out.append(Vector3(xw, -worldY, cy));
            out.append(Vector3(cx, -worldY, yh));
            out.append(Vector3(cx, -worldY, cy));

            break;
        }
        break;
    }
    case Right:
    {
        return;

        if (!rn2)
        {
            /* add remainders */
            if (rn1 > 0)
            {
                out.append(Vector3(-size.x, worldY, yh)); /* 1 */
                out.append(Vector3(-size.x, -worldY, cy)); /* 2 */
                out.append(Vector3(-size.x, -worldY, yh)); /* 3 */


                out.append(Vector3(-size.x, worldY, yh)); /* 1 */
                out.append(Vector3(-size.x, -worldY, yh)); /* 3 */
                out.append(Vector3(-size.x, worldY, yh)); /* 0 */
            }
        }
        else
        {

        }
        break;
    }
    }


    /* if theres any texturing problems, it's definitely coming from here */

    Vector2 u = Vector2(), v = Vector2();

    float realX = 4, realZ = 2;

    if (rn0 > 0)
    {
        realX = rn0;
        realZ = height;
    }

    if (rn1 > 0)
    {
        realX = width;
        realZ = rn1;
    }

    if (rn2 > 0)
    {
        realX = rn0;
        realZ = rn1;
    }

    Vector3 rSize = Vector3(realX, worldY, realZ);
    Vector2 sSize = PartInstance::getSubdivisionNumbers(face, rSize);

    Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sSize, u, v);

    appendTexCoordsXYWH(texCoordsOut, u, v, sSize, sliceW, sliceH);
}

void PartInstance::rbxSubdivide(SurfaceType surface, NormalId face, int width, int height, float w, float y, float h, Vector2 u, Vector2 v, CoordinateFrame cframe, Array<Vector3>& out, Array<Vector2>& texCoordsOut)
{

    if (h < height)
    {
        height = h;
    }

    if (w < width)
    {
        width = w;
        if (w > height) {
            width = height;
        }
    }

    float sliceX = floor(w / width);
    float sliceY = floor(h / height);

    float remainderStripX = fmodf(w, width);
    float remainderStripY = fmodf(h, height);
    float remainderCorner = remainderStripX + remainderStripY; /* last remainder square */

    float uX = width;
    float vY = height;

    float trueY = y / 2;

    float originX = (-w / 2);
    float originY = (-h / 2);

    if (sliceX == 0)
    {
        sliceX = remainderStripX;
    }
    if (sliceY == 0)
    {
        sliceY = remainderStripY;
    }

    out.clear();
    texCoordsOut.clear();

    if (G3D::isFinite(sliceX) && G3D::isFinite(sliceY))
    {
        /* actual stuff okay */

        for (float y = 0; y < sliceY; y++)
        {
            for (float x = 0; x < sliceX; x++)
            {
                float cx = originX + x * width;
                float cy = originY + y * height;

                float xw = cx + width;
                float yh = cy + height;

                Vector3 rSize = Vector3(uX, trueY, vY);
                Vector2 sSize = Vector2();

                if (y == sliceY - 1)
                {
                    if (rSize.z < height)
                    {
                        yh += remainderStripY;
                        rSize.z += remainderStripY;
                    }
                }

                getSliceInformation(face, surface, rSize, sSize, u, v);
                rbxAppendProductSlice(face, surface, sSize, u, v, cx, cy, xw, yh, trueY, w, h, out, texCoordsOut);

                if (x == sliceX - 1 || y == sliceY - 1)
                {
                    float remainder0XWidth = xw + remainderStripX;
                    float remainder0YHeight = yh;

                    float remainder1XWidth = xw;
                    float remainder1YHeight = yh + remainderStripY;

                    if (x == sliceX - 1)
                    {
                        if (remainderStripX > 0)
                        {
                            rbxAppendRemainderSlice(face, surface, sSize, remainderStripX, 0, 0, cx, cy, xw, yh, remainder0XWidth, remainder0YHeight, 0, trueY, w, h, width, height, out, texCoordsOut);
                        }
                    }
                    if (y == sliceY - 1)
                    {
                        if (remainderStripY > 0 && (rSize.z >= height))
                        {
                            rbxAppendRemainderSlice(face, surface, sSize, 0, remainderStripY, 0, cx, cy, xw, yh, remainder1XWidth, remainder1YHeight, 0, trueY, w, h, width, height, out, texCoordsOut);
                        }
                    }
                    if (x == sliceX - 1 && y == sliceY - 1)
                    {
                        float rh = y * height;
                        if (rh == 0)
                        {
                            rh = height;
                        }
                        if (rh > 0)
                        {
                            if (rh != h)
                            {
                                if (remainderStripX > 0)
                                {
                                    rbxAppendRemainderSlice(face, surface, sSize,
                                        remainderStripX,
                                        remainderStripY,
                                        1,
                                        cx,
                                        cy,
                                        xw,
                                        yh,
                                        remainder0XWidth,
                                        remainder0YHeight + remainderStripY,
                                        0, trueY, w, h, width, height, out, texCoordsOut);
                                }
                            }
                        }
                    }
                }
            }

        }
    }

}