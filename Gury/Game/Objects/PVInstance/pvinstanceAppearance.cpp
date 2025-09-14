
#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"

using namespace RBX;

Vector3 RBX::PVInstance::getQuadFaceNormal(RBX::NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
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

void RBX::PVInstance::offsetBrickFaceVertices(Array<Vector3>& vertices, Vector3 offset)
{
    if (vertices.size() == 6)
    {
        vertices[0] = vertices[0] + offset;
        vertices[1] = vertices[1] + offset;
        vertices[2] = vertices[2] + offset;
        vertices[3] = vertices[3] + offset;
        vertices[4] = vertices[4] + offset;
        vertices[5] = vertices[5] + offset;
    }
}

Array<Vector3> PVInstance::getBrickFaceVertices(NormalId face, bool asWorldSpace)
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

    //SurfaceType surface = getSurface(face);
    SurfaceType surface = Smooth;

    Vector3 sSize = size * 2;
    Vector2 subdivisions = getSubdivisionNumbers(face, sSize);

    Vector2 u, v; /* get Base UV */

    Render::TextureReserve::TexturePositionalInformation faceUV = getFaceUv(face);

    u = Vector2(faceUV.x, faceUV.y);
    v = Vector2(faceUV.cx, faceUV.cy);

    Array<Vector3> vertices = getBrickFaceVertices(face, false);
    CoordinateFrame cframe = getCenter();
    
    if (surface != Smooth)
    {
        if (subdivisions.x <= 2 && subdivisions.y <= 4 || 
            subdivisions.x <= 4 && subdivisions.y <= 2)
        {
            goto final;
        }

        if (subdivisions.x > 30 || subdivisions.y > 30)
        {
            /* Mega texture */

            faceUV = Render::TextureReserve::get()->getSurfaceUV(color, Smooth, face, getSubdivisionNumbers(face, size * 2));

            u = Vector2(faceUV.x, faceUV.y);
            v = Vector2(faceUV.cx, faceUV.cy);

            goto final;

            Vector3 v0 = vertices[0];
            Vector3 v1 = vertices[1];
            Vector3 v2 = vertices[2];
            Vector3 v3 = vertices[5]; /* as a quad - skip two */

            int n = ceil(subdivisions.x) / 2;

            rbxSubdivide(surface, face, n, sSize.y, 1, u, v, cframe, out, texCoordsOut);

            return;
        }
        else
        {

            Vector3 v0 = vertices[0];
            Vector3 v1 = vertices[1];
            Vector3 v2 = vertices[2];
            Vector3 v3 = vertices[5]; /* as a quad - skip two */

            int n0 = subdivisions.y;
            int n1 = subdivisions.x;

            rbxSubdivide(surface, face, n0, sSize.y, n1, u, v, cframe, out, texCoordsOut);

            return;
        }
    }
    else
    {
final:

        out = vertices;

        appendTexCoordsXYWH(texCoordsOut, u, v, subdivisions);

        return;
    }
}

Render::TextureReserve::TexturePositionalInformation RBX::PVInstance::getFaceUv(NormalId face)
{
    /* do getSurface for actual surfaces */
    return Render::TextureReserve::get()->getSurfaceUV(color, Smooth, face, getSubdivisionNumbers(face, size * 2));
}

void RBX::PVInstance::removeSurfaces()
{
    BrickColor::getColorMap()->tryRemove(color, getFrontSurface());
    BrickColor::getColorMap()->tryRemove(color, getBackSurface());
    BrickColor::getColorMap()->tryRemove(color, getTopSurface());
    BrickColor::getColorMap()->tryRemove(color, getBottomSurface());
    BrickColor::getColorMap()->tryRemove(color, getLeftSurface());
    BrickColor::getColorMap()->tryRemove(color, getRightSurface());
}

void RBX::PVInstance::orderSurfaces()
{
    BrickColor::getColorMap()->orderInAtlas(color, getFrontSurface());
    BrickColor::getColorMap()->orderInAtlas(color, getBackSurface());
    BrickColor::getColorMap()->orderInAtlas(color, getTopSurface());
    BrickColor::getColorMap()->orderInAtlas(color, getBottomSurface());
    BrickColor::getColorMap()->orderInAtlas(color, getLeftSurface());
    BrickColor::getColorMap()->orderInAtlas(color, getRightSurface());
}

void RBX::PVInstance::updateWholeFace(NormalId normalId)
{
    if (vertexIndices.containsKey(normalId))
    {
        Face storedFace = vertexIndices.get(normalId);

        removeFaceFromProxy(currentProxy, storedFace);
        vertexIndices.remove(normalId);

        writeBrickFace(normalId);

        addFaceToProxy(currentProxy, vertexIndices.get(normalId));
    }
}

void RBX::PVInstance::updateWholeFaces()
{
    Array<NormalId> faces = vertexIndices.getKeys();
    for (int i = 0; i < faces.size(); i++)
    {
        NormalId face = faces[i];
        updateWholeFace(face);
    }
}

void RBX::PVInstance::regenerateRenderable()
{
    if(shape == Block)
    {
        updateWholeFaces();
        edit();
    }
    else
    {
        removeFromRenderEnvironment();
        if (specialShape)
        {
            specialShape->removeFromRenderEnvironment();
        }
        write();
        edit();
    }
}

void RBX::PVInstance::step()
{
    if (specialShape)
    {
        specialShape->edit();
    }
    else
    {
        switch (shape)
        {
        case Cylinder:
        case Block:
        {
            editBrickFaces();
            break;
        }
        case Ball:
        {
            sphereRadius = size.y;
            editBrickFaces();
            break;
        }
        }
    }
}
