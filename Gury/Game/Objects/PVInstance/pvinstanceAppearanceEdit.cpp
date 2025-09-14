#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/Rendering/textureReserve.h"

void RBX::PVInstance::editBrickFaces()
{
    CoordinateFrame current = getCoordinateFrame();

    Array<NormalId> keys = vertexIndices.getKeys();
    Array<Vector3> vertices = Render::Mesh::getGlobalMesh()->originVertexArray;

    for (int j = 0; j < keys.size(); j++)
    {
        NormalId face = keys[j];
        Face storedFace = vertexIndices.get(face);

        for (int i = 0; i < storedFace.indices.size(); i++)
        {
            Vector3 vertex;
            uint32 idx = storedFace.indices[i];

            vertex = vertices[idx];
            vertex = current.pointToWorldSpace(vertex);

            Render::Mesh::editVertex(idx, vertex);
        }

    }
}

void RBX::PVInstance::editBrickFaceGeometry(RBX::NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit)
{
    Vector3 normal = getQuadFaceNormal(face, v0, v1, v2, v3, v4, v5);

    if (vertexIndices.containsKey(face))
    {
        Face storedFace = vertexIndices.get(face);

        if (storedFace.indices.size() == 6)
        {
            Vector2 u, v;

            u = storedFace.u;
            v = storedFace.v;

            Render::Mesh::edit(storedFace.indices[0], v0, normal, u);
            Render::Mesh::edit(storedFace.indices[1], v1, normal, Vector2(u.x, u.y + v.y));
            Render::Mesh::edit(storedFace.indices[2], v2, normal, u + v);
            Render::Mesh::edit(storedFace.indices[3], v3, normal, u);
            Render::Mesh::edit(storedFace.indices[4], v4, normal, u + v);
            Render::Mesh::edit(storedFace.indices[5], v5, normal, Vector2(u.x + v.x, u.y));
        }

    }
}