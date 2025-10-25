#include "pvinstance.h"

void RBX::PVInstance::writeBrickFace(NormalId face)
{
    Array<Vector3> subdivided;
    Array<Vector2> subdividedTexcoords;

    generateSubdividedFace(subdivided, subdividedTexcoords, face);
    writeBrickGeometry(face, subdivided, subdividedTexcoords);
}

void RBX::PVInstance::writeBrickGeometry(NormalId face, Array<Vector3> vertices, Array<Vector2> texCoords)
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
