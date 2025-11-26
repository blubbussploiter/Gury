
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Rendering/renderMesh.h"

#include "part.h"
#include "mesh.h"

/* do this later */

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Render::SpecialMesh>("SpecialMesh")
		.constructor<>()
		.property("Scale", &RBX::Render::SpecialMesh::getMeshScale, &RBX::Render::SpecialMesh::setMeshScale)(rttr::metadata("Type", RBX::Data))
		.property("MeshId", &RBX::Render::SpecialMesh::getMeshId, &RBX::Render::SpecialMesh::setMeshId)
		.property("MeshType", &RBX::Render::SpecialMesh::getMeshType, &RBX::Render::SpecialMesh::setMeshType)(rttr::metadata("Type", RBX::Data));
	rttr::registration::enumeration<RBX::Render::MeshType>("MeshType")
		(
			rttr::value("Head", RBX::Render::Head),
			rttr::value("Torso", RBX::Render::Torso),
			rttr::value("Wedge", RBX::Render::Wedge),
			rttr::value("Sphere", RBX::Render::Sphere),
			rttr::value("Cylinder", RBX::Render::Cylinder),
			rttr::value("FileMesh", RBX::Render::Filemesh),
			rttr::value("Brick", RBX::Render::Brick)
		);

}

void RBX::Render::SpecialMesh::writeFace(NormalId face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
{
	/* 10/19 outdated
	Vector3 normal = normalize((v0 - v2).cross(v1 - v3).direction());

	meshIndices.push_back(RBX::Render::Mesh::write(v0, normal, xy));
	meshIndices.push_back(RBX::Render::Mesh::write(v1, normal, Vector2(xy.x, xy.y + wh.y)));
	meshIndices.push_back(RBX::Render::Mesh::write(v2, normal, xy + wh));
	meshIndices.push_back(RBX::Render::Mesh::write(v3, normal, xy));
	meshIndices.push_back(RBX::Render::Mesh::write(v4, normal, xy + wh));
	meshIndices.push_back(RBX::Render::Mesh::write(v5, normal, Vector2(xy.x + wh.x, xy.y)));
	*/

}

void RBX::Render::SpecialMesh::editFace(NormalId face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5)
{
	/* 10/19 outdated 
	Vector3 normal = normalize((v0 - v2).cross(v1 - v3).direction());

	if (vertexIndices.containsKey(face))
	{
		Face _face = vertexIndices[face];

		RBX::Render::Mesh::edit(meshIndices[0], v0, normal, xy);
		RBX::Render::Mesh::edit(_face.indices[1], v1, normal, Vector2(xy.x, xy.y + wh.y));
		RBX::Render::Mesh::edit(_face.indices[2], v2, normal, xy + wh);
		RBX::Render::Mesh::edit(_face.indices[3], v3, normal, xy);
		RBX::Render::Mesh::edit(_face.indices[4], v4, normal, xy + wh);
		RBX::Render::Mesh::edit(_face.indices[5], v5, normal, Vector2(xy.x + wh.x, xy.y));
	}
	*/
}

void RBX::Render::SpecialMesh::writeTriangleFace(NormalId face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2)
{
	/* 10/19 outdated
	Vector3 normal = normalize((v0 - v1).cross(v1 - v2).direction());

	if (!vertexIndices.containsKey(face))
	{
		Face newFace;

		newFace.indices.push_back(RBX::Render::Mesh::write(v0, normal, xy));
		newFace.indices.push_back(RBX::Render::Mesh::write(v1, normal, Vector2(xy.x, xy.y + wh.y)));
		newFace.indices.push_back(RBX::Render::Mesh::write(v2, normal, xy + wh));

		vertexIndices.set(face, newFace);
	}
	*/
}

void RBX::Render::SpecialMesh::editTriangleFace(NormalId face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2)
{
	/* 10/19 outdated
	Vector3 normal = normalize((v0 - v1).cross(v1 - v2).direction());

	if (vertexIndices.containsKey(face))
	{
		Face _face = vertexIndices[face];

		RBX::Render::Mesh::edit(_face.indices[0], v0, normal, xy);
		RBX::Render::Mesh::edit(_face.indices[1], v1, normal, Vector2(xy.x, xy.y + wh.y));
		RBX::Render::Mesh::edit(_face.indices[2], v2, normal, xy + wh);
	}
	*/
}

void RBX::Render::SpecialMesh::fromMeshType(MeshType types)
{
	setMeshType(types);
}

void RBX::Render::SpecialMesh::resizeMesh()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = vertices[i] * mesh_scale;
	}
	edit(); /* reposition what not */
}

void RBX::Render::SpecialMesh::fromFile(std::string path)
{
	FILE* f = fopen(path.c_str(), "r");

	float vx, vy, vz,
		nx, ny, nz,
		ux, uy, uz;

	fscanf(f, "version 1.00\n");
	fscanf(f, "%d\n", &faces);

	for (int i = 0; i < faces; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fscanf(f, "[%f,%f,%f][%f,%f,%f][%f,%f,%f]", &vx, &vy, &vz, &nx, &ny, &nz, &ux, &uy, &uz);
			vx *= 0.5;
			vy *= 0.5;
			vz *= 0.5;
			uy = 1.0 - uy;
			vertices.push_back(Vector3(vx, vy, vz));
			normals.push_back(Vector3(nx, ny, nz));
			uvs.push_back(Vector3(ux, uy, uz));
		}
	}

	fclose(f);

	num_faces = faces * 3;
	meshId = Content::fromStoredContent(path);
}

void RBX::Render::SpecialMesh::setMeshId(Content SpecialMeshId)
{
	std::string contentPath;

	if (meshType != Filemesh)
	{
		return;
	}

	/* something... */

	vertices.clear();
	normals.clear();
	uvs.clear();

	fromFile(contentPath);
}

void RBX::Render::SpecialMesh::write()
{
	PartInstance* part = toInstance<PartInstance>(getParent());

	if (part)
	{
		/* Set our renderable thingies */

		transparency = part->transparency;
		reflectance = part->reflectance;
		color = part->color;
		alpha = part->alpha;

		switch (meshType)
		{
			case Head:{
				writeHead();
				break;
			}
			case Wedge:
			{
				writeWedge();
				break;
			}
			default:
			{
				writeSpecialMesh();
				break;
			}
		}
		editGlobalProxyLocation();
	}
}

void RBX::Render::SpecialMesh::edit()
{
	PartInstance* part = toInstance<PartInstance>(getParent());
	if (part)
	{
		/* Set our renderable thingies */

		transparency = part->transparency;
		reflectance = part->reflectance;
		alpha = part->alpha;

		switch (meshType)
		{
			case Head: {
				editHead();
				break;
			}
			case Wedge:
			{
				editWedge();
				break;
			}
			default:
			{
				editSpecialMesh();
				break;
			}
		}
		editGlobalProxyLocation();
	}
}

void RBX::Render::SpecialMesh::onParentChanged(Instance* self, rttr::property property)
{
	std::string propertyName = property.get_name().to_string();

	if (propertyName == "Parent") {

		Instance* parent = self->parent;

		if (parent) {

			PVInstance* pvInstance = toInstance<PVInstance>(parent);

			if (pvInstance) {

				IRenderable* renderableSelf = toInstance<IRenderable>(self);
				parent->onChanged.connect(onParentSizeChanged);
				pvInstance->removeFromRenderEnvironment();
				pvInstance->specialShape = renderableSelf;
				renderableSelf->write();
			}
		}
	}
}

void RBX::Render::SpecialMesh::onParentSizeChanged(Instance* self, rttr::property property)
{
	std::string propertyName = property.get_name().to_string();

	if (self && propertyName == "Size")
	{
		PVInstance* pvInstance = toInstance<PVInstance>(self);
		if (pvInstance->specialShape)
		{
			RBX::StandardOut::print(RBX::MESSAGE_INFO, "done with");
			pvInstance->specialShape->removeFromRenderEnvironment();
			pvInstance->specialShape->write();
		}
	}
}
