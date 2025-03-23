
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Rendering/renderMesh.h"

#include "part.h"
#include "mesh.h"

/* do this later */

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::Render::SpecialMesh>("SpecialMesh")
		.constructor<>()
		.property("Scale", &RBX::Render::SpecialMesh::getMeshScale, &RBX::Render::SpecialMesh::setMeshScale)
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

void RBX::Render::SpecialMesh::fromMeshType(MeshType types)
{
	setMeshType(types);
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
	meshId = Content::fromContent(path);
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
		alpha = part->alpha;

		switch (meshType)
		{
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
	}
}

void RBX::Render::buildHeadMesh(Vector3 size)
{

}
