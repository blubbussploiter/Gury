
#include "PVInstance/pvinstance.h"

#include "mesh.h"

void RBX::Render::HeadMesh::buildTube(Vector3 from, Vector3 to, int slices, Array<Vector3>& vertices, Array<Vector3>& normals)
{
	float radius1 = from.z;
	float radius2 = to.z;
	for (int i = 0; i < slices; i++)
	{
		float theta = float(i) * 2 * pi() / slices;
		float nextTheta = float(i+1) * 2 * pi() / slices;

		float x1 = cos(theta);
		float z1 = sin(theta);
		float x2 = cos(nextTheta);
		float z2 = sin(nextTheta);

		Vector3 v0 = Vector3(radius1 * x1, from.y, radius1 * z1);
		Vector3 v1 = Vector3(radius1 * x2, from.y, radius1 * z2);
		Vector3 v2 = Vector3(radius2 * x2, to.y, radius2 * z2);
		Vector3 v3 = Vector3(radius2 * x1, to.y, radius2 * z1);

		vertices.push_back(v0);
		vertices.push_back(v1);
		vertices.push_back(v2);

		vertices.push_back(v0);
		vertices.push_back(v2);
		vertices.push_back(v3);

		normals.push_back(normalize(v0));
		normals.push_back(normalize(v1));
		normals.push_back(normalize(v2));

		normals.push_back(normalize(v0));
		normals.push_back(normalize(v2));
		normals.push_back(normalize(v3));
	}
}

void RBX::Render::HeadMesh::buildCircle(float circleRadius, float circleY, int triangles, Array<Vector3>& vertices, Array<Vector3>& normals, bool invertSecondVertex)
{
	Array<Vector3> circleVertices;

	/* allocate top circle */

	for (int i = 0; i < triangles; i++) {
		float angle = (360.0f / triangles) * i;
		float x = circleRadius * cos(toRadians(angle));
		float z = circleRadius * sin(toRadians(angle));
		circleVertices.push_back(Vector3(x, circleY, z));
	}

	/* add top circle */

	for (int i = 0; i < triangles - 2; i++) {
		vertices.push_back(circleVertices[0]);
		vertices.push_back(circleVertices[i + 1+invertSecondVertex]);
		vertices.push_back(circleVertices[i + 2-invertSecondVertex]);

		normals.push_back(normalize(circleVertices[0]));
		normals.push_back(normalize(circleVertices[i + 1 + invertSecondVertex]));
		normals.push_back(normalize(circleVertices[i + 2 - invertSecondVertex]));
	}

}

void RBX::Render::HeadMesh::getFaceVertices(Vector3 size, NormalId face, Array<Vector3>& vertices, Array<Vector3>& normals)
{
	int triangles = 32;

	float radius = std::min(size.x, size.z);
	float circleRadius = radius * 0.5f;

	/* Build top circle */

	buildCircle(circleRadius, size.y, triangles, vertices, normals, true);

	/* Bevels leading on */

	/* Curve 1 */
	buildTube(
		Vector3(circleRadius, size.y, circleRadius),
		Vector3(radius * 0.675f, size.y*0.98f, radius * 0.675f),
		triangles, vertices, normals);

	/* Curve 2 */
	buildTube(
		Vector3(radius * 0.675f, size.y*0.98f, radius * 0.675f),
		Vector3(radius * 0.85f, size.y*0.9f, radius * 0.85f),
		triangles, vertices, normals);

	/* Curve 3 */
	buildTube(
		Vector3(radius * 0.85f, size.y * 0.9f, radius * 0.85f),
		Vector3(radius, size.y * 0.7f, radius),
		triangles, vertices, normals);

	/* Center */

	buildTube(
		Vector3(radius, size.y * 0.7f, radius),
		Vector3(radius * 1.025f, -size.y * 0.5f, radius*1.025f),
		triangles, vertices, normals);

	/* Bevels leading off */

	/* Curve 1 */

	buildTube(
		Vector3(radius * 1.025f, -size.y * 0.5f, radius * 1.025f),
		Vector3(radius * 1.005f, -size.y * 0.7f, radius * 1.005f),
		triangles, vertices, normals);

	/* Curve 2 */

	buildTube(
		Vector3(radius * 1.005f, -size.y * 0.7f, radius * 1.005f),
		Vector3(radius * 0.925f, -size.y * 0.85f, radius * 0.925f),
		triangles, vertices, normals);

	/* Curve 3 */

	buildTube(
		Vector3(radius * 0.925f, -size.y * 0.85f, radius * 0.925f),
		Vector3(radius * 0.68f, -size.y * 0.98f, radius * 0.68f),
		triangles, vertices, normals);
	
	/* Curve 4 */

	buildTube(
		Vector3(radius * 0.68f, -size.y * 0.98f, radius * 0.68f),
		Vector3(circleRadius, -size.y, circleRadius),
		triangles, vertices, normals);

	/* Build bottom circle */

	buildCircle(circleRadius, -size.y, triangles, vertices, normals, false);

}

void RBX::Render::SpecialMesh::writeHead()
{
	PVInstance* part = toInstance<PVInstance>(getParent());
	if (part)
	{
		CoordinateFrame position = part->getCoordinateFrame();
		
		if (!vertexIndices.containsKey(UNDEFINED))
		{
			Face newFace;

			HeadMesh::getFaceVertices(getMeshScaleBySize(), UNDEFINED, vertices, normals);

			for (int i = 0; i < vertices.size(); i++)
			{
				Vector3 vertex = vertices[i];
				Vector3 normal = normals[i];

				Vector3 newVertex = position.pointToWorldSpace(vertex);
				newFace.indices.push_back(RBX::Render::Mesh::write(newVertex, normal, Vector2(), color));
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

		if (vertexIndices.containsKey(UNDEFINED))
		{
			Face storedFace = vertexIndices.get(UNDEFINED);

			if (storedFace.indices.size() == vertices.size())
			{
				for (int i = 0; i < storedFace.indices.size(); i++)
				{
					uint32 index = storedFace.indices[i];

					Vector3 vertex = vertices[i];
					Vector3 normal = normals[i];

					Vector3 newVertex = position.pointToWorldSpace(vertex);
					RBX::Render::Mesh::edit(index, newVertex, normal, Vector2(), color);

				}

			}

		}
	}
}

void RBX::Render::SpecialMesh::removeFromRenderEnvironment()
{
	vertices.clear();
	normals.clear();
	uvs.clear();
	IRenderable::removeFromRenderEnvironment();
}
