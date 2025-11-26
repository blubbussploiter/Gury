
#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"

Vector3 RBX::PVInstance::getQuadFaceNormal(NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
{
    Vector3 v0v1 = v0 - v2;
    Vector3 v0v2 = v1 - v2;
    Vector3 v3v4 = v3 - v5;
    Vector3 v3v5 = v4 - v5;
    Vector3 normal = (v0v1.cross(v0v2) + v3v4.cross(v3v5)).direction();
    
    if (face != Left)
    {
        return normal;
    }
    else
    {
        return Vector3(-normal.x, normal.y, normal.z);
   }
}

Array<Vector3> RBX::PVInstance::calculateBrickFaceVertices(NormalId face, Vector3 size)
{
    Array<Vector3> vertices;
    switch (face)
    {
    case NormalId::Top:
    {
        vertices.append(Vector3(size.x, size.y, -size.z)); /* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, -size.z)); /* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 3 */
        break;
    }
    case NormalId::Bottom:
    {
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */

        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 3 */
        break;
    }
    case NormalId::Front:
    {
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 1 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 3 */
        break;
    }
    case NormalId::Back:
    {
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, size.z)); /* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 3 */
        break;
    }
    case NormalId::Left:
    {
        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(size.x, size.y, size.z));/* vertex 1 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 2 */

        vertices.append(Vector3(size.x, size.y, -size.z));/* vertex 0 */
        vertices.append(Vector3(size.x, -size.y, size.z));/* vertex 2 */
        vertices.append(Vector3(size.x, -size.y, -size.z));/* vertex 3 */
        break;
    }
    case NormalId::Right:
    {
        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, size.y, -size.z));/* vertex 1 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 2 */

        vertices.append(Vector3(-size.x, size.y, size.z));/* vertex 0 */
        vertices.append(Vector3(-size.x, -size.y, -size.z));/* vertex 2 */
        vertices.append(Vector3(-size.x, -size.y, size.z));/* vertex 3 */
        break;
    }
    }
    return vertices;
}

Array<Vector3> RBX::PVInstance::getBrickFaceVertices(NormalId face, bool asWorldSpace)
{
    Array<Vector3> vertices = calculateBrickFaceVertices(face, size);

    if (asWorldSpace)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            CoordinateFrame coord = getCoordinateFrame();
            vertices[i] = coord.pointToWorldSpace(vertices[i]);
        }
    }
    return vertices;
}

Vector2 RBX::PVInstance::getSubdivisionNumbers(NormalId face, Vector3 size)
{
    switch (face)
    {
        case Top:
        case Bottom:
        {
            return Vector2(size.z, size.x);
        }
        case Front:
        case Back:
        {
            return Vector2(size.x, size.y);
        }
        case Left:
        case Right:
        {
            return Vector2(size.z, size.y);
        }
    }
    return Vector2(1, 1);
}


void RBX::PVInstance::generateSubdividedFace(Array<Vector3>& out, Array<Vector2>& texCoordsOut, NormalId face)
{

    Render::TextureReserve* textureReserve = Render::TextureReserve::get();
    SurfaceType surface = getSurface(face);

    Vector3 worldSize = size * 2;

    Array<Vector3> vertices = getBrickFaceVertices(face, false);
    Vector2 subdivisions = getSubdivisionNumbers(face, worldSize);

    Render::TextureReserve::TexturePositionalInformation faceUV = textureReserve->getSurfaceUV(color, surface, face, subdivisions);
    
    /* get Base UV */

    Vector2 u = Vector2(faceUV.x, faceUV.y);
    Vector2 v = Vector2(faceUV.cx, faceUV.cy);
    
    if (surface != Smooth)
    {

        /* Subdivided medium sized brick */

        if (subdivisions.x < 30 && subdivisions.y < 30) 
        {
            int subDivisionWidth = 2;
            int subDivisionHeight = 4;

            float width = subdivisions.y;
            float height = subdivisions.x;

            rbxSubdivide(surface, face, subDivisionWidth, subDivisionHeight, width, worldSize.y, height, u, v, getCenter(), out, texCoordsOut);
        }
        else /* Mega texture */
        {

            float width = subdivisions.y;
            float height = subdivisions.x;

            float subDivisionWidth = width;
            float subDivisionHeight = 2;

            int iWidth = floorf(subDivisionWidth);
            int iHeight = floorf(subDivisionHeight);

            rbxSubdivide(surface, face, iWidth, iHeight, width, worldSize.y, height, u, v, getCenter(), out, texCoordsOut);
        }
    }
    else
    {
        /* Smooth brick */
        out = vertices;
        appendTexCoordsXYWH(texCoordsOut, u, v, subdivisions, 4, 2);
    }
}

void RBX::PVInstance::removeSurfaces(Color4 surfaceColor)
{
    removeSurface(surfaceColor, front);
    removeSurface(surfaceColor, back);
    removeSurface(surfaceColor, top);
    removeSurface(surfaceColor, bottom);
    removeSurface(surfaceColor, left);
    removeSurface(surfaceColor, right);
}

void RBX::PVInstance::orderSurfaces(Color4 surfaceColor)
{
    orderSurface(surfaceColor, front);
    orderSurface(surfaceColor, back);
    orderSurface(surfaceColor, top);
    orderSurface(surfaceColor, bottom);
    orderSurface(surfaceColor, left);
    orderSurface(surfaceColor, right);
}

void RBX::PVInstance::orderSurface(Color4 surfaceColor, SurfaceType surfaceType)
{
    BrickColor::getColorMap()->orderInAtlas(surfaceColor, surfaceType);
}

void RBX::PVInstance::removeSurface(Color4 surfaceColor, SurfaceType surfaceType)
{
    BrickColor::getColorMap()->tryRemove(surfaceColor, surfaceType);
}

void RBX::PVInstance::reorderSurfaces(Color4 oldColor)
{
    /* Reorder surfaces */
    removeSurfaces(oldColor);
    orderSurfaces(color);
}

void RBX::PVInstance::regenerateRenderable()
{
    if (meshIndices.size() > 0)
    {
        removeFromRenderEnvironment();
    }
    doWrite(true);
    edit();
}

void RBX::PVInstance::step()
{
    if (specialShape)
    {
        specialShape->edit();
    }
    else
    {
        editMeshPositition();
    }
}
