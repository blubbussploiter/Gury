
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
		Vector3 vertex = vertices.at(i) * mesh_scale, worldVertex;
		Vector3 normal = normals.at(i).direction();
		Vector3 uv = uvs.at(i);

		worldVertex = newPosition.pointToWorldSpace(vertex);

		face.indices.push_back(RBX::Render::Mesh::write(vertex, normal, Vector2(uv.x, uv.y), color));
	}

	vertexIndices.set(UNDEFINED, face);

	editGlobalProxyLocation();
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
			newPosition = toInstance<PartInstance>(parent)->getCoordinateFrame();

			for (int i = 0; i < num_faces; i++)
			{
				int index = face.indices[i];
				Vector3 vertex = vertices.at(i) * mesh_scale, worldVertex;
				Vector3 normal = normals.at(i).direction();
				Vector3 uv = uvs.at(i);

				worldVertex = newPosition.pointToWorldSpace(vertex);

				RBX::Render::Mesh::edit(index, worldVertex, normal, Vector2(uv.x, uv.y), color);
			}

		}
	}
	else
	{
		writeSpecialMesh();
	}

	editGlobalProxyLocation();
}
