#include "pvinstance.h"

#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/Rendering/textureReserve.h"

void RBX::PVInstance::editMeshPositition()
{
    CoordinateFrame current = getCoordinateFrame();

    Array<Vector3> vertices = Render::Mesh::getGlobalMesh()->originVertexArray;

    for (int i = 0; i < meshIndices.size(); i++)
    {
        Vector3 vertex;
        uint32 idx = meshIndices[i];

        vertex = vertices[idx];
        vertex = current.pointToWorldSpace(vertex);

        Render::Mesh::editVertex(idx, vertex);
    }

}