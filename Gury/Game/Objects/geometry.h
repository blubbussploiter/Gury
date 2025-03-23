#pragma once

#include <G3DAll.h>

#include "PVInstance/pvenums.h"

namespace RBX
{
	namespace Render
	{
		class Geometry
		{
		public:

			CoordinateFrame cframe;
			Vector3 size;

			RBX::Shape shape;

			Box getBox()
			{
				Box box = Box(-size, size);
				return cframe.toWorldSpace(box);
			}

			G3D::Sphere getSphere()
			{
				return G3D::Sphere(cframe.translation, size.y);
			}

			Geometry() {
				shape = Block;
				size = Vector3::zero();
			}
			Geometry(CoordinateFrame cframe, Vector3 size, Shape shape) : cframe(cframe), size(size), shape(shape) {}
		};
	}
}