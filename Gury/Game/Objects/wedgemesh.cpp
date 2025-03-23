#include "mesh.h"
#include "part.h"

void RBX::Render::WedgeMesh::writeFace(NormalId face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
{
	Vector3 normal = normalize((v0 - v2).cross(v1 - v3).direction());

	if (!vertexIndices.containsKey(face))
	{
		Face newFace;

		newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v3, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v4, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v5, normal, uv, color));

		vertexIndices.set(face, newFace);
	}
}

void RBX::Render::WedgeMesh::editFace(NormalId face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
{
	Vector3 normal = normalize((v0 - v2).cross(v1 - v3).direction());

	if (vertexIndices.containsKey(face))
	{
		Face _face = vertexIndices[face];

		RBX::Render::Mesh::edit(_face.indices[0], v0, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[1], v1, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[2], v2, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[3], v3, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[4], v4, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[5], v5, normal, uv, color);
	}
}

void RBX::Render::WedgeMesh::writeTriangleFace(NormalId face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2)
{
	Vector3 normal = normalize((v0 - v1).cross(v1 - v2).direction());

	if (!vertexIndices.containsKey(face))
	{
		Face newFace;

		newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, uv, color));
		newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, uv, color));

		vertexIndices.set(face, newFace);
	}
}

void RBX::Render::WedgeMesh::editTriangleFace(NormalId face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2)
{
	Vector3 normal = normalize((v0 - v1).cross(v1 - v2).direction());

	if (vertexIndices.containsKey(face))
	{
		Face _face = vertexIndices[face];

		RBX::Render::Mesh::edit(_face.indices[0], v0, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[1], v1, normal, uv, color);
		RBX::Render::Mesh::edit(_face.indices[2], v2, normal, uv, color);
	}
}

Array<Vector3> RBX::Render::WedgeMesh::getFaceVertices(CoordinateFrame position, Vector3 size, NormalId face)
{
	Array<Vector3> vertices;
	switch (face)
	{
		case Bottom:
		{
			vertices.push_back(Vector3(size.x, -size.y, size.z)); /* vertex 0 */
			vertices.push_back(Vector3(-size.x, -size.y, size.z)); /* vertex 1 */
			vertices.push_back(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */

			vertices.push_back(Vector3(size.x, -size.y, size.z)); /* vertex 0 */
			vertices.push_back(Vector3(-size.x, -size.y, -size.z)); /* vertex 2 */
			vertices.push_back(Vector3(size.x, -size.y, -size.z)); /* vertex 3 */
			break;
		}
		case Front:
		{
			vertices.push_back(Vector3(-size.x, -size.y, -size.z)); /* vertex 0 */
			vertices.push_back(Vector3(-size.x, size.y, size.z)); /* vertex 1 */
			vertices.push_back(Vector3(size.x, size.y, size.z)); /* vertex 2 */

			vertices.push_back(Vector3(-size.x, -size.y, -size.z)); /* vertex 0 */
			vertices.push_back(Vector3(size.x, size.y, size.z)); /* vertex 2 */
			vertices.push_back(Vector3(size.x, -size.y, -size.z)); /* vertex 3 */
			break;
		}
		case Back:
		{
			vertices.push_back(Vector3(size.x, size.y, size.z));/* vertex 0 */
			vertices.push_back(Vector3(-size.x, size.y, size.z));/* vertex 1*/
			vertices.push_back(Vector3(-size.x, -size.y, size.z));/* vertex 2 */

			vertices.push_back(Vector3(size.x, size.y, size.z));/* vertex 0 */
			vertices.push_back(Vector3(-size.x, -size.y, size.z));/* vertex 2 */
			vertices.push_back(Vector3(size.x, -size.y, size.z));/* vertex 3 */
			break;
		}
		case Left:
		{
			vertices.push_back(Vector3(-size.x, -size.y, -size.z));
			vertices.push_back(Vector3(-size.x, -size.y, size.z));
			vertices.push_back(Vector3(-size.x, size.y, size.z));
			break;
		}
		case Right:
		{
			vertices.push_back(Vector3(size.x, -size.y, size.z));
			vertices.push_back(Vector3(size.x, -size.y, -size.z));
			vertices.push_back(Vector3(size.x, size.y, size.z));
			break;
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = position.pointToWorldSpace(vertices[i]);
	}
	return vertices;

}

void RBX::Render::WedgeMesh::writeWedgeFace(NormalId face)
{
	PartInstance* part = toInstance<PartInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();
		Vector2 uv0(size.x, -size.z / 2), uv1, uv2;

		Array<Vector3> vertices = getFaceVertices(part->getCoordinateFrame(), size, face);

		if (face != Top)
		{
			switch (face)
			{
			case Left:
			case Right:
			{
				writeTriangleFace(face, uv2, vertices[0], vertices[1], vertices[2]);
				break;
			}
			default:
			{
				writeFace(face, uv1, vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]);
				break;
			}
			}
		}
	}
}

void RBX::Render::WedgeMesh::editWedgeFace(NormalId face)
{
	PartInstance* part = toInstance<PartInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();
		Vector2 uv0(size.x, -size.z / 2), uv1, uv2;

		Array<Vector3> vertices = getFaceVertices(part->getCoordinateFrame(), size, face);

		if (face != Top)
		{
			switch (face)
			{
			case Left:
			case Right:
			{
				editTriangleFace(face, uv2, vertices[0], vertices[1], vertices[2]);
				break;
			}
			default:
			{
				editFace(face, uv1, vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]);
				break;
			}
			}
		}
	}
}

void RBX::Render::WedgeMesh::writeWedge()
{
	writeWedgeFace(Right);
	writeWedgeFace(Left);
	writeWedgeFace(Back);
	writeWedgeFace(Bottom);
	writeWedgeFace(Front);
	editGlobalProxyLocation();
}

void RBX::Render::WedgeMesh::editWedge()
{
	editWedgeFace(Right);
	editWedgeFace(Left);
	editWedgeFace(Back);
	editWedgeFace(Bottom);
	editWedgeFace(Front);
	editGlobalProxyLocation();
}