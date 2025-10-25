
#include "../Gury/Game/Rendering/renderMesh.h"

#include "mesh.h"
#include "part.h"

void RBX::Render::SpecialMesh::writeSpecialMesh()
{
	CoordinateFrame newPosition;
	color = BrickColor::BrickMap::get()->fromName("Medium stone grey");

	if (IsA<PartInstance>(parent))
	{
		color = toInstance<PartInstance>(parent)->color;
		newPosition = toInstance<PartInstance>(parent)->getCoordinateFrame();
	}

	for (size_t i = 0; i < num_faces; i++)
	{
		Vector3 vertex = vertices[i], worldVertex;
		Vector3 uv = uvs[i];

		worldVertex = newPosition.pointToWorldSpace(vertex);
		Vector3 normal = normalize(worldVertex - newPosition.translation);

		meshIndices.push_back(RBX::Render::Mesh::write(vertex, normal, Vector2(uv.x, uv.y)));
	}

}

void RBX::Render::SpecialMesh::editSpecialMesh()
{
	RBX::Render::Mesh* mesh = RBX::Render::Mesh::getGlobalMesh();

	if (meshIndices.size() > 0)
	{
		CoordinateFrame newPosition;
		if (IsA<PartInstance>(parent))
		{
			color = toInstance<PartInstance>(parent)->color;
			newPosition = toInstance<PartInstance>(parent)->getCoordinateFrame();

			for (int i = 0; i < num_faces; i++)
			{
				int index = meshIndices[i];
				Vector3 vertex = vertices[i], worldVertex;
				Vector3 uv = uvs[i];

				worldVertex = newPosition.pointToWorldSpace(vertex);
				Vector3 normal = normalize(worldVertex - newPosition.translation);

				RBX::Render::Mesh::edit(index, worldVertex, normal, Vector2(uv.x, uv.y));
			}

		}
	}
	else
	{
		writeSpecialMesh();
	}
}