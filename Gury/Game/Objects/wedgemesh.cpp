#include "mesh.h"
#include "part.h"


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

void RBX::Render::SpecialMesh::writeWedgeFace(NormalId face)
{
	PartInstance* part = toInstance<PartInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();
		SurfaceType surface = part->getSurface(face);

		Array<Vector3> vertices = WedgeMesh::getFaceVertices(part->getCoordinateFrame(), size, face);
		Vector2 subdividedTileSize = PVInstance::getSubdivisionNumbers(face, size);

		Render::TextureReserve::TexturePositionalInformation faceUV = TextureReserve::get()->getSurfaceUV(Color3(color), surface, face, subdividedTileSize);

		if (face != Top)
		{
			switch (face)
			{
			case Left:
			case Right:
			{
				writeTriangleFace(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy), vertices[0], vertices[1], vertices[2]);
				break;
			}
			default:
			{
				writeFace(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy), vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]);
				break;
			}
			}
		}
	}
}

void RBX::Render::SpecialMesh::editWedgeFace(NormalId face)
{
	PartInstance* part = toInstance<PartInstance>(getParent());
	if (part)
	{

		Vector3 size = part->getSize();
		SurfaceType surface = part->getSurface(face);

		Array<Vector3> vertices = WedgeMesh::getFaceVertices(part->getCoordinateFrame(), size, face);
		Vector2 subdividedTileSize = PVInstance::getSubdivisionNumbers(face, size);

		Render::TextureReserve::TexturePositionalInformation faceUV = TextureReserve::get()->getSurfaceUV(Color3(color), surface, face, subdividedTileSize);

		if (face != Top)
		{
			switch (face)
			{
			case Left:
			case Right:
			{
				editTriangleFace(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy), vertices[0], vertices[1], vertices[2]);
				break;
			}
			default:
			{
				editFace(face, Vector2(faceUV.x, faceUV.y), Vector2(faceUV.cx, faceUV.cy), vertices[0], vertices[1], vertices[2], vertices[3], vertices[4], vertices[5]);
				break;
			}
			}
		}
	}
}


void RBX::Render::SpecialMesh::writeWedge()
{
	writeWedgeFace(Right);
	writeWedgeFace(Left);
	writeWedgeFace(Back);
	writeWedgeFace(Bottom);
	writeWedgeFace(Front);
}

void RBX::Render::SpecialMesh::editWedge()
{
	editWedgeFace(Right);
	editWedgeFace(Left);
	editWedgeFace(Back);
	editWedgeFace(Bottom);
	editWedgeFace(Front);
}