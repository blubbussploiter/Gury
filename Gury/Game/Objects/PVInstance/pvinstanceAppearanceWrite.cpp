#include "pvinstance.h"

void RBX::PVInstance::writeBrickFace(NormalId face, bool repeat)
{
    Array<Vector3> subdivided;
    Array<Vector2> subdividedTexcoords;

    generateSubdividedFace(subdivided, subdividedTexcoords, face);
    
    writeBrickFaceGeometryNew(face, subdivided, subdividedTexcoords, color);
}

void RBX::PVInstance::writeBrickFaceGeometry(RBX::NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit)
{
    Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3, v4 ,v5);

    if (!vertexIndices.containsKey(face))
    {
        Render::TextureReserve::TexturePositionalInformation faceUV = getFaceUv(face);
        Face newFace;

        Vector2 subdivisions = getSubdivisionNumbers(face, size);

        Vector2 xy = Vector2(faceUV.x, faceUV.y);
        Vector2 wh = Vector2(faceUV.cx, faceUV.cy);

        newFace.u = xy;
        newFace.v = wh;

        newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, xy));
        newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, Vector2(xy.x, xy.y + wh.y)));
        newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, xy + wh));

        newFace.indices.push_back(RBX::Render::Mesh::write(v3, normal, xy));
        newFace.indices.push_back(RBX::Render::Mesh::write(v4, normal, xy + wh));
        newFace.indices.push_back(RBX::Render::Mesh::write(v5, normal, Vector2(xy.x + wh.x, xy.y)));

        vertexIndices.set(face, newFace);
    }
}

void RBX::PVInstance::writeBrickFaceGeometryNew(RBX::NormalId face, Array<Vector3> vertices, Array<Vector2> texCoords, Color4 color)
{
    if (!vertexIndices.containsKey(face))
    {
        Render::TextureReserve::TexturePositionalInformation faceUV = getFaceUv(face);
        Face newFace;

        CoordinateFrame current = getCenter();

        for (int i = 0; i < vertices.size()-5; i+=6)
        {

            Vector3 v0, v1, v2, v3, v4, v5;

            v0 = vertices[i];
            v1 = vertices[i + 1];
            v2 = vertices[i + 2];
            v3 = vertices[i + 3];
            v4 = vertices[i + 4];
            v5 = vertices[i + 5];

            Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3, v4, v5);

            newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, texCoords[i]));
            newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, texCoords[i + 1]));
            newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, texCoords[i + 2]));

            newFace.indices.push_back(RBX::Render::Mesh::write(v3, normal, texCoords[i + 3]));
            newFace.indices.push_back(RBX::Render::Mesh::write(v4, normal, texCoords[i + 4]));
            newFace.indices.push_back(RBX::Render::Mesh::write(v5, normal, texCoords[i + 5]));


        }

        vertexIndices.set(face, newFace);
    }
}
