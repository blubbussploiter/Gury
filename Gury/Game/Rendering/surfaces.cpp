
#include "surfaceFactory.h"
#include "../Objects/part.h"

GImage RBX::getSurfaceFromFile(RBX::SurfaceType s)
{
	GImage surfaces;
	GImage surface;

	float y = 0;

	std::string fn = GetFileInPath("/content/textures/GurySurfaces.png");

	surfaces = GImage();
	surfaces.load(fn);

	switch (s)
	{
	case RBX::Weld:
	{
		y = 256;
		break;
	}
	case RBX::Glue:
	{
		y = 384;
		break;
	}
	case RBX::Studs:
	{
		y = 0;
		break;
	}
	case RBX::Inlet:
	{
		y = 128;
		break;
	}
	}

	surfaces.copySubImage(surface, surfaces, 0, y, 64, 128);
	return surface;
}

CoordinateFrame RBX::getSurfaceCenter(NormalId face, Vector3 size, Extents extents)
{
	CoordinateFrame center;
	Vector3 extentsCenter;

	extentsCenter = extents.getCenter();

	switch (face)
	{
	case NormalId::Front:
	{
		center = extentsCenter + Vector3(0, 0, -size.z);
		center.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(90));
		break;
	}
	case NormalId::Back:
	{
		center = extentsCenter + Vector3(0, 0, size.z);
		center.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(90));
		break;
	}
	case NormalId::Left:
	{
		center = extentsCenter + Vector3(-size.x, 0, 0);
		break;
	}
	case NormalId::Right:
	{
		center = extentsCenter + Vector3(size.x, 0, 0);
		break;
	}
	case NormalId::Top:
	{
		center = extentsCenter + Vector3(0, size.y, 0);
		center.rotation = Matrix3::fromAxisAngle(Vector3::unitZ(), toRadians(90));
		break;
	}
	case NormalId::Bottom:
	{
		center = extentsCenter + Vector3(0, -size.y, 0);
		center.rotation = Matrix3::fromAxisAngle(Vector3::unitZ(), toRadians(90));
		break;
	}
	}

	return center;
}