#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/Rendering/textureReserve.h"

void RBX::PVInstance::editBrickFace(NormalId face, int unit, bool repeat)
{
    Array<Vector3> faceVertices = getBrickFaceVertices(face);
    Render::TextureReserve::TexturePositionalInformation faceUV = getFaceUv(face);

    editBrickFaceGeometry(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy),
        faceVertices[0], faceVertices[1], faceVertices[2], faceVertices[3], faceVertices[4], faceVertices[5],
        color, unit);
}

void RBX::PVInstance::editBrickFaces()
{
    if (!specialShape)
    {
        switch (shape)
        {
        case Block: /* go through each face, edit to new position */
        {
            editBrickFace(Right, -1);
            editBrickFace(Top, -1);
            editBrickFace(Back, -1);
            editBrickFace(Left, -1);
            editBrickFace(Bottom, -1);
            editBrickFace(Front, -1);

            editSurfaces();
            break;
        }
        }
    }
}

void RBX::PVInstance::editBrickFaceGeometry(RBX::NormalId face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit)
{
    auto reserve = Render::TextureReserve::get();
    Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3);

    if (vertexIndices.containsKey(face))
    {
        Face storedFace = vertexIndices.get(face);

        if (storedFace.indices.size() == 6)
        {
            Render::Mesh::edit(storedFace.indices[0], v0, normal, xy, color, textureUnit);
            Render::Mesh::edit(storedFace.indices[1], v1, normal, Vector2(xy.x, xy.y + wh.y), color, textureUnit);
            Render::Mesh::edit(storedFace.indices[2], v2, normal, xy + wh, color, textureUnit);
            Render::Mesh::edit(storedFace.indices[3], v3, normal, xy, color, textureUnit);
            Render::Mesh::edit(storedFace.indices[4], v4, normal, xy + wh, color, textureUnit);
            Render::Mesh::edit(storedFace.indices[5], v5, normal, Vector2(xy.x + wh.x, xy.y), color, textureUnit);
        }

    }
}

void RBX::PVInstance::editSurfaces()
{
    if (!specialShape)
    {
        editSurface(Right, idRight);
        editSurface(Top, idTop);
        editSurface(Back, idBack);
        editSurface(Left, idLeft);
        editSurface(Bottom, idBottom);
        editSurface(Front, idFront);
    }
}

void RBX::PVInstance::editSurface(NormalId face, unsigned int unit)
{
    Color4 color = Color4(1, 1, 1, alpha - 0.2f);
    if (unit != -1)
    {
        /* change textureunit */
    }
}