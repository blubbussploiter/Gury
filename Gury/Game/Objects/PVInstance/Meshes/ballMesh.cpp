
#include "icosphere.hpp"
#include "../../part.h"

void RBX::PartInstance::writeBall()
{
	std::vector<Vector3> vertices_temp, vertices;
	std::vector<uint32_t> indices;

	/* generate */
	
	generateIcosphereMesh(2, indices, vertices_temp);
	reorder(sphereRadius, indices, vertices_temp, vertices);

	vertices_temp.clear();
	indices.clear();

	Vector2 ru, rv;

	Render::TextureReserve::get()->getSurfaceXXYY(color, Smooth, UNDEFINED, Vector2(1, 1), ru, rv);

	for (int i = 0; i < vertices.size(); i++)
	{
		Vector3 vertex = vertices[i];
		Vector2 uv = (ru);

		Vector3 n = normalize(vertex);

		meshIndices.push_back(RBX::Render::Mesh::write(vertex, n, uv));
	}
}
