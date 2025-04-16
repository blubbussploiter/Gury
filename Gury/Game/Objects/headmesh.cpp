
#include "PVInstance/pvinstance.h"

#include "mesh.h"

Array<Vector3> RBX::Render::HeadMesh::getFaceVertices(CoordinateFrame position, Vector3 size, NormalId face)
{
	Array<Vector3> vertices;
	Array<Vector3> circleVertices;

	int triangles = 12;
	float radius = size.z;

	float circleRadius = radius / 2;

	/* allocate top circle */

	for (int i = 0; i < triangles; i++) {
		float angle = (360.0f / triangles) * i;
		float x = circleRadius * cos(toRadians(angle));
		float z = circleRadius * sin(toRadians(angle));
		circleVertices.push_back(Vector3(x, size.y, z));
	}

	/* add top circle */

	for (int i = 0; i < triangles - 2; i++) {
		vertices.push_back(circleVertices[0]);
		vertices.push_back(circleVertices[i + 2]);
		vertices.push_back(circleVertices[i + 1]);
	}

	circleVertices.clear();

	/* Try out */


	/* allocate bottom circle */

	for (int i = 0; i < triangles; i++) {
		float angle = (360.0f / triangles) * i;
		float x = circleRadius * cos(toRadians(angle));
		float z = circleRadius * sin(toRadians(angle));
		circleVertices.push_back(Vector3(x, -size.y, z));
	}

	/* add bottom circle */

	for (int i = 0; i < triangles - 2; i++) {
		vertices.push_back(circleVertices[0]);
		vertices.push_back(circleVertices[i + 1]);
		vertices.push_back(circleVertices[i + 2]);
	}

	circleVertices.clear();

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = position.pointToWorldSpace(vertices[i]);
	}

	return vertices;
}


void RBX::Render::SpecialMesh::writeHead()
{
	PVInstance* part = toInstance<PVInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();

		CoordinateFrame position = part->getCoordinateFrame();
		Array<Vector3> vertices = HeadMesh::getFaceVertices(position, size, UNDEFINED);

		if (!vertexIndices.containsKey(UNDEFINED))
		{
			Face newFace;

			for (int i = 0; i < vertices.size(); i++)
			{
				Vector3 vertex = vertices[i];
				Vector3 normal = normalize(vertex - position.translation);;
				newFace.indices.push_back(RBX::Render::Mesh::write(vertex, normal, Vector2(), color));
			}

			vertexIndices.set(UNDEFINED, newFace);
		}
	}
}

void RBX::Render::SpecialMesh::editHead()
{
	PVInstance* part = toInstance<PVInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();

		CoordinateFrame position = part->getCoordinateFrame();
		Array<Vector3> vertices = HeadMesh::getFaceVertices(position, size, UNDEFINED);

		if (vertexIndices.containsKey(UNDEFINED))
		{
			Face storedFace = vertexIndices.get(UNDEFINED);

			if (storedFace.indices.size() == vertices.size())
			{
				for (int i = 0; i < storedFace.indices.size(); i++)
				{
					uint32 index = storedFace.indices[i];

					Vector3 vertex = vertices[i];
					Vector3 normal = normalize(vertex - position.translation);
					RBX::Render::Mesh::edit(index, vertex, normal, Vector2(), color);

				}

			}

		}
	}
}
