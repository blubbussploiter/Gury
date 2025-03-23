#include "pvinstance.h"

void RBX::PVInstance::writeBrickFace(NormalId face, int unit, bool repeat)
{
    Array<Vector3> faceVertices = getBrickFaceVertices(face);
    Render::TextureReserve::TexturePositionalInformation faceUV = getFaceUv(face);

    writeBrickFaceGeometry(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy),
        faceVertices[0], faceVertices[1], faceVertices[2], faceVertices[3], faceVertices[4], faceVertices[5],
        color, unit);
}

void RBX::PVInstance::writeBrickFaceGeometry(RBX::NormalId face,Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit)
{
    Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3);

    if (!vertexIndices.containsKey(face))
    {
        Face newFace;

        newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, xy, color, textureUnit));
        newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, Vector2(xy.x, xy.y + wh.y), color, textureUnit));
        newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, xy + wh, color, textureUnit));

        newFace.indices.push_back(RBX::Render::Mesh::write(v3, normal, xy, color, textureUnit));
        newFace.indices.push_back(RBX::Render::Mesh::write(v4, normal, xy + wh, color, textureUnit));
        newFace.indices.push_back(RBX::Render::Mesh::write(v5, normal, Vector2(xy.x + wh.x, xy.y), color, textureUnit));

        vertexIndices.set(face, newFace);
    }
}

void RBX::PVInstance::writeSurfaces()
{
    if (!specialShape)
    {
        /* 0 - 5 again */

        writeSurface(Right, idRight);
        writeSurface(Top, idTop);
        writeSurface(Back, idBack);
        writeSurface(Left, idLeft);
        writeSurface(Bottom, idBottom);
        writeSurface(Front, idFront);
        
    }
}

void RBX::PVInstance::writeSurface(NormalId face, unsigned int unit)
{
    /*
        SurfaceFactory::orderSurface(brickColor) blah blah blah 
    */
}