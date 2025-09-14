#include "pvenums.h"
#include "pvinstance.h"

using namespace RBX;

void PVInstance::appendTexCoordsXYWH(Array<Vector2>& texCoordsOut, Vector2 ru, Vector2 rv, Vector2 size)
{
    texCoordsOut.push_back(ru);
    texCoordsOut.push_back(Vector2(ru.x + rv.x, ru.y));
    texCoordsOut.push_back(ru + rv);

    texCoordsOut.push_back(ru);
    texCoordsOut.push_back(ru + rv);
    texCoordsOut.push_back(Vector2(ru.x, ru.y + rv.y));
}

void PVInstance::rbxSubdivide(SurfaceType surface, NormalId face, int w, float y, int h, Vector2 u, Vector2 v, CoordinateFrame cframe, Array<Vector3>& out, Array<Vector2>& texCoordsOut)
{

    /* probably the worst code i've written in my life */

    int height = 2;
    int width = 4;

    float cn0 = w / width;
    float cn1 = h / height;

    float rn0 = w % width;
    float rn1 = h % height;
    float rn2 = rn0 + rn1; /* last remainder square */

    float tY = y / 2;

    float originX = (-w / 2);
    float originY = (-h / 2);

    if (rn0 > 0 && rn0 != 2)
    {
        if (rn0 == 3)
        {
            originX -= 0.5f;
        }
        else
        {
            originX -= rn0 / 2;
        }
    }
    if (rn1 > 0 &&
        rn1 != 2)
    {
        if (rn1 == 3)
        {
            originY -= 0.5f;
        }
        else
        {
            originY -= rn1 / 2;
        }
    }

    if (cn0 == 0)
    {
        cn0 = rn0;
    }
    if (cn1 == 0)
    {
        cn1 = rn1;
    }

    float sX = height;
    float sY = width;

    if (w < width)
    {
        sY = w;
        cn1 = 1;
        rn0 = 0;
        rn2 = 0;
    }

    if (h < height)
    {
        sX = h;
        cn0 = 1;
        rn1 = 0;
        rn2 = 0;
    }

    Vector3 rSize = Vector3(sX, tY, sY);
    Vector2 sSize = PVInstance::getSubdivisionNumbers(face, rSize);

    Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sSize, u, v);

    /* dont look at this code ignore it  */

    texCoordsOut.clear();

    /* actual stuff okay */

    for (int y = 0; y < cn1; y++)
    {
        for (int x = 0; x < cn0; x++)
        {

            /* offset */

            float xStart = originX + x * sX;
            float yStart = originY + y * sY;

            float xWidth = xStart + sX;
            float yHeight = yStart + sY;

            /* uhhh */

            Vector3 cv0, cv1, cv2, cv3;
            Vector3 xrv0, xrv1, xrv2, xrv3,
                yrv0, yrv1, yrv2, yrv3,
                _yrv0, _yrv1, _yrv2, _yrv3;

            Vector2 ru, rv;

            float remainder0XWidth = xWidth + rn0;
            float remainder0YHeight = yHeight;

            float remainder1XWidth = xWidth;
            float remainder1YHeight = yHeight + rn1;

            float remainder2XWidth = remainder0XWidth;
            float remainder2YHeight = yHeight + (rn2 - 1);

            if (face == Top)
            {
                cv0 = Vector3(xStart, tY, yHeight);
                cv1 = Vector3(xWidth, tY, yHeight);
                cv2 = Vector3(xWidth, tY, yStart);
                cv3 = Vector3(xStart, tY, yStart);

                /* add remainder */

                if (rn0 > 0)
                {
                    xrv0 = Vector3(xWidth, tY, remainder0YHeight);
                    xrv1 = Vector3(remainder0XWidth, tY, remainder0YHeight);
                    xrv2 = Vector3(remainder0XWidth, tY, yStart);
                    xrv3 = Vector3(xWidth, tY, yStart);
                }
                if (rn1 > 0)
                {
                    yrv0 = Vector3(xStart, tY, remainder1YHeight);
                    yrv1 = Vector3(remainder1XWidth, tY, remainder1YHeight);
                    yrv2 = Vector3(remainder1XWidth, tY, yHeight);
                    yrv3 = Vector3(xStart, tY, yHeight);
                }

                /* add third remainder */

                if (rn2 > 0)
                {
                    _yrv0 = Vector3(xWidth, tY, remainder2YHeight);
                    _yrv1 = Vector3(remainder2XWidth, tY, remainder2YHeight);
                    _yrv2 = Vector3(remainder2XWidth, tY, yHeight);
                    _yrv3 = Vector3(xWidth, tY, yHeight);
                }
            }
            else if (face == Bottom)
            {
                cv3 = Vector3(xStart, -tY, yHeight);
                cv2 = Vector3(xWidth, -tY, yHeight);
                cv1 = Vector3(xWidth, -tY, yStart);
                cv0 = Vector3(xStart, -tY, yStart);

                /* add remainder */

                if (rn0 > 0)
                {
                    xrv3 = Vector3(xWidth, -tY, remainder0YHeight);
                    xrv2 = Vector3(remainder0XWidth, -tY, remainder0YHeight);
                    xrv1 = Vector3(remainder0XWidth, -tY, yStart);
                    xrv0 = Vector3(xWidth, -tY, yStart);
                }
                if (rn1 > 0)
                {
                    yrv3 = Vector3(xStart, -tY, remainder1YHeight);
                    yrv2 = Vector3(remainder1XWidth, -tY, remainder1YHeight);
                    yrv1 = Vector3(remainder1XWidth, -tY, yHeight);
                    yrv0 = Vector3(xStart, -tY, yHeight);
                }

                /* add third remainder */

                if (rn2 > 0)
                {
                    _yrv3 = Vector3(xWidth, -tY, remainder2YHeight);
                    _yrv2 = Vector3(remainder2XWidth, -tY, remainder2YHeight);
                    _yrv1 = Vector3(remainder2XWidth, -tY, yHeight);
                    _yrv0 = Vector3(xWidth, -tY, yHeight);
                }
            }
            else if (face == Right)
            {
                cv3 = Vector3(-size.x, -tY, yHeight);
                cv2 = Vector3(-size.x, -tY, yStart);
                cv1 = Vector3(-size.x, tY, yStart);
                cv0 = Vector3(-size.x, tY, yHeight);

                /* add remainder */

                if (rn1 > 0)
                {
                    xrv3 = Vector3(-size.x, -tY, remainder1YHeight);
                    xrv2 = Vector3(-size.x, -tY, yHeight); 
                    xrv1 = Vector3(-size.x, tY, yHeight);
                    xrv0 = Vector3(-size.x, tY, remainder1YHeight);
                }
            }
            else if (face == Left)
            {
                cv0 = Vector3(size.x, -tY, yHeight);
                cv1 = Vector3(size.x, -tY, yStart);
                cv2 = Vector3(size.x, tY, yStart);
                cv3 = Vector3(size.x, tY, yHeight);
            }

            out.append(cv1);
            out.append(cv2);
            out.append(cv3);

            out.append(cv1);
            out.append(cv3);
            out.append(cv0);

            appendTexCoordsXYWH(texCoordsOut, u, v, sSize);

            continue;
            
            if (x == cn0 - 1 || y == cn1 - 1)
            {
                if (y == cn1 - 1)
                {
                    if (rn1 > 0)
                    {
                        Vector3 rSize = Vector3(sX, tY, rn1);
                        Vector2 sSize = PVInstance::getSubdivisionNumbers(face, rSize);

                        Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sSize, ru, rv);

                        out.append(yrv1);
                        out.append(yrv2);
                        out.append(yrv3);

                        out.append(yrv1);
                        out.append(yrv3);
                        out.append(yrv0);

                        appendTexCoordsXYWH(texCoordsOut, ru, rv, sSize);
                    }
                }

                if (x == cn0 - 1)
                {
                    if (rn0 > 0)
                    {
                        Vector3 rSize = Vector3(rn0, tY, sY);
                        Vector2 sSize = PVInstance::getSubdivisionNumbers(face, rSize);

                        Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sSize, ru, rv);

                        out.append(xrv1);
                        out.append(xrv2);
                        out.append(xrv3);

                        out.append(xrv1);
                        out.append(xrv3);
                        out.append(xrv0);

                        appendTexCoordsXYWH(texCoordsOut, ru, rv, sSize);
                    }
                }

                if (x == cn0 - 1 && y == cn1 - 1)
                {
                    if (rn2 > 0)
                    {
                        Vector3 rSize = Vector3(rn0, tY, rn2 - 1);
                        Vector2 sSize = PVInstance::getSubdivisionNumbers(face, rSize);

                        Render::TextureReserve::get()->getSurfaceXXYY(color, surface, face, sSize, ru, rv);

                        out.append(_yrv1);
                        out.append(_yrv2);
                        out.append(_yrv3);

                        out.append(_yrv1);
                        out.append(_yrv3);
                        out.append(_yrv0);

                        appendTexCoordsXYWH(texCoordsOut, ru, rv, sSize);
                    }

                }

            }
        }

    }
}