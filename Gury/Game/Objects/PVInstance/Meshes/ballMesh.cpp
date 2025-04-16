
#include "../pvinstance.h"

int sectors = 28;
int stacks = 20;

Array<Vector3> RBX::PVInstance::getBallFaceVertices()
{
	Array<Vector3> vertices;

	float latitude0 = 0, longitude0 = 0, latitude1 = 360, longitude1 = 360;
	float radius = sphereRadius;

	float mnlat = toRadians(latitude0);
	float mnlon = toRadians(longitude0);
	float mxlon = toRadians(longitude1);
	float mxlat = toRadians(latitude1);

	for (int i = 0; i < stacks; ++i)
	{
		float t0 = mnlat + (mxlat - mnlat) * i / stacks;
		float t1 = mnlat + (mxlat - mnlat) * (i + 1) / stacks;
		for (int j = 0; j < sectors; ++j)
		{
			float p0 = mnlon + (mxlon - mnlon) * (j) / sectors;
			float p1 = mnlon + (mxlon - mnlon) * (j + 1) / sectors;

			Vector3 v1, v2, v3, v4;

			v1 = Vector3(
				radius * cos(t0) * cos(p0),
				radius * sin(t0),
				radius * cos(t0) * sin(p0));
			v2 = Vector3(
				radius * cos(t0) * cos(p1),
				radius * sin(t0),
				radius * cos(t0) * sin(p1));
			v3 = Vector3(
				radius * cos(t1) * cos(p0),
				radius * sin(t1),
				radius * cos(t1) * sin(p0));
			v4 = Vector3(
				radius * cos(t1) * cos(p1),
				radius * sin(t1),
				radius * cos(t1) * sin(p1));

			vertices.append(v1, v3, v2);
			vertices.append(v2, v3, v4);
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		CoordinateFrame coord = getCoordinateFrame();
		vertices[i] = coord.pointToWorldSpace(vertices[i]);
	}
	return vertices;
}

void RBX::PVInstance::writeBall()
{
	Array<Vector3> vertices = getBallFaceVertices();

	if (!vertexIndices.containsKey(UNDEFINED))
	{
		Face newFace;

		for (int i = 0; i < vertices.size(); i++)
		{
			Vector3 vertex = vertices[i];
			Vector3 normal = normalize(vertex - getPosition());
			newFace.indices.push_back(RBX::Render::Mesh::write(vertex, normal, Vector2(), color));
		}

		vertexIndices.set(UNDEFINED, newFace);
	}

}

void RBX::PVInstance::editBall()
{
	Array<Vector3> vertices = getBallFaceVertices();

	if (vertexIndices.containsKey(UNDEFINED))
	{
		Face storedFace = vertexIndices.get(UNDEFINED);

		if (storedFace.indices.size() == vertices.size())
		{
			for (int i = 0; i < storedFace.indices.size(); i++)
			{
				uint32 index = storedFace.indices[i];

				Vector3 vertex = vertices[i];
				Vector3 normal = normalize(vertex - getPosition());

				RBX::Render::Mesh::edit(index, vertex, normal, Vector2(), color);

			}

		}

	}
}
