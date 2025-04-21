
#include "../Gury/Game/Rendering/renderMesh.h"

#include "mesh.h"
#include "part.h"

void RBX::Render::SpecialMesh::writeSpecialMesh()
{
	Face face;

	CoordinateFrame newPosition;
	color = BrickColor::BrickMap::get()->fromName("Medium stone grey");

	if (IsA<PartInstance>(parent))
	{
		color = toInstance<PartInstance>(parent)->color;
		newPosition = toInstance<PartInstance>(parent)->getCoordinateFrame();
	}

	for (size_t i = 0; i < num_faces; i++)
	{
		Vector3 vertex = vertices[i] * getMeshScaleBySize(), worldVertex;
		Vector3 uv = uvs[i];

		worldVertex = newPosition.pointToWorldSpace(vertex);
		Vector3 normal = normalize(worldVertex - newPosition.translation);

		face.indices.push_back(RBX::Render::Mesh::write(vertex, normal, Vector2(uv.x, uv.y), color));
	}

	vertexIndices.set(UNDEFINED, face);
}

void RBX::Render::SpecialMesh::editSpecialMesh()
{
	RBX::Render::Mesh* mesh = RBX::Render::Mesh::getGlobalMesh();

	if (vertexIndices.containsKey(UNDEFINED))
	{
		Face face = vertexIndices[UNDEFINED];

		CoordinateFrame newPosition;
		if (IsA<PartInstance>(parent))
		{
			color = toInstance<PartInstance>(parent)->color;
			newPosition = toInstance<PartInstance>(parent)->getCoordinateFrame();

			for (int i = 0; i < num_faces; i++)
			{
				int index = face.indices[i];
				Vector3 vertex = vertices[i] * getMeshScaleBySize(), worldVertex;
				Vector3 uv = uvs[i];

				worldVertex = newPosition.pointToWorldSpace(vertex);
				Vector3 normal = normalize(worldVertex - newPosition.translation);

				RBX::Render::Mesh::edit(index, worldVertex, normal, Vector2(uv.x, uv.y), color);
			}

		}
	}
	else
	{
		writeSpecialMesh();
	}
}