
#include "part.h"

#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/Objects/mesh.h"

void RBX::PartInstance::editMeshPosition()
{
    CoordinateFrame current = getCoordinateFrame();

    for (int i = 0; i < meshIndices.size(); i++)
    {
        Vector3 vertex;
        uint32 idx = meshIndices[i];

        vertex = Render::Mesh::getGlobalMesh()->originalVertex(idx);
        vertex = current.pointToWorldSpace(vertex);

        Render::Mesh::editVertex(idx, vertex);
    }

}
void RBX::PartInstance::writeBrickFace(NormalId face)
{
    Array<Vector3> subdivided;
    Array<Vector2> subdividedTexcoords;

    generateSubdividedFace(subdivided, subdividedTexcoords, face);
    writeBrickGeometry(face, subdivided, subdividedTexcoords);
}

void RBX::PartInstance::writeBrickGeometry(NormalId face, Array<Vector3> vertices, Array<Vector2> texCoords)
{
    for (int i = 0; i < vertices.size() - 5; i += 6)
    {

        Vector3 v0, v1, v2, v3, v4, v5;

        v0 = vertices[i];
        v1 = vertices[i + 1];
        v2 = vertices[i + 2];
        v3 = vertices[i + 3];
        v4 = vertices[i + 4];
        v5 = vertices[i + 5];

        Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3, v4, v5);

        meshIndices.push_back(RBX::Render::Mesh::write(v0, normal, texCoords[i]));
        meshIndices.push_back(RBX::Render::Mesh::write(v1, normal, texCoords[i + 1]));
        meshIndices.push_back(RBX::Render::Mesh::write(v2, normal, texCoords[i + 2]));

        meshIndices.push_back(RBX::Render::Mesh::write(v3, normal, texCoords[i + 3]));
        meshIndices.push_back(RBX::Render::Mesh::write(v4, normal, texCoords[i + 4]));
        meshIndices.push_back(RBX::Render::Mesh::write(v5, normal, texCoords[i + 5]));
    }

}


Vector3 RBX::PartInstance::getQuadFaceNormal(NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
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

Array<Vector3> RBX::PartInstance::calculateBrickFaceVertices(NormalId face, Vector3 size)
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

Array<Vector3> RBX::PartInstance::getBrickFaceVertices(NormalId face, bool asWorldSpace)
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

Vector2 RBX::PartInstance::getSubdivisionNumbers(NormalId face, Vector3 size)
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


void RBX::PartInstance::generateSubdividedFace(Array<Vector3>& out, Array<Vector2>& texCoordsOut, NormalId face)
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

            TileBuilder::tile(color, worldSize, surface, face, subDivisionWidth, subDivisionHeight, width, worldSize.y, height, u, v, getCenter(), out, texCoordsOut);
        }
        else /* Mega texture */
        {

            float width = subdivisions.y;
            float height = subdivisions.x;

            float subDivisionWidth = width;
            float subDivisionHeight = 2;

            int iWidth = floorf(subDivisionWidth);
            int iHeight = floorf(subDivisionHeight);

            TileBuilder::tile(color, worldSize, surface, face, iWidth, iHeight, width, worldSize.y, height, u, v, getCenter(), out, texCoordsOut);
        }
    }
    else
    {
        /* Smooth brick */
        out = vertices;
        TileBuilder::appendTexCoordsXYWH(texCoordsOut, u, v, subdivisions, 4, 2);
    }
}

void RBX::PartInstance::removeSurfaces(Color4 surfaceColor)
{
    removeSurface(surfaceColor, front);
    removeSurface(surfaceColor, back);
    removeSurface(surfaceColor, top);
    removeSurface(surfaceColor, bottom);
    removeSurface(surfaceColor, left);
    removeSurface(surfaceColor, right);
}

void RBX::PartInstance::orderSurfaces(Color4 surfaceColor)
{
    orderSurface(surfaceColor, front);
    orderSurface(surfaceColor, back);
    orderSurface(surfaceColor, top);
    orderSurface(surfaceColor, bottom);
    orderSurface(surfaceColor, left);
    orderSurface(surfaceColor, right);
}

void RBX::PartInstance::orderSurface(Color4 surfaceColor, SurfaceType surfaceType)
{
    BrickColor::getColorMap()->orderInAtlas(surfaceColor, surfaceType);
}

void RBX::PartInstance::removeSurface(Color4 surfaceColor, SurfaceType surfaceType)
{
    BrickColor::getColorMap()->tryRemove(surfaceColor, surfaceType);
}

bool RBX::PartInstance::containSurface(SurfaceType surface)
{
    return (top == surface || bottom == surface || right == surface || left == surface || front == surface || back == surface);
}

void RBX::PartInstance::reorderSurfaces(Color4 oldColor)
{
    /* Reorder surfaces */
    removeSurfaces(oldColor);
    orderSurfaces(color);
}

void RBX::PartInstance::regenerateRenderable()
{
    Render::SpecialMesh * mesh = getSpecialMesh();
    if (mesh)
    {
        mesh->removeFromRenderEnvironment();
    }
    else
    { 
        if (meshIndices.size() > 0)
        {
            removeFromRenderEnvironment();
        }
    }
    doWrite(true);
    edit();
}

void RBX::PartInstance::regenerateFace(NormalId face)
{
    Array<Vector3> subdivided;
    Array<Vector2> subdividedTexcoords;

    generateSubdividedFace(subdivided, subdividedTexcoords, face);

    for (int i = 0; i < meshIndices.size(); i++)
    {
        Vector3 vertex, normal;
        uint32 idx = meshIndices[i];

        vertex = Render::Mesh::getGlobalMesh()->vertexArray[idx];
        normal = Render::Mesh::getGlobalMesh()->normalArray[idx];

        Render::Mesh::edit(idx, vertex, normal, subdividedTexcoords[i]);
    }

}
