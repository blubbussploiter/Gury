#pragma once

#include <G3DAll.h>
#include "ode/ode.h"

#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Reflection/signal.h"

#include "../Gury/Game/Objects/geometry.h"

#include "body.h"

namespace RBX
{
	class Primitive
	{
	public:

		RBX::Shape shape;

		dGeomID geom[1];

		Body* body;
		PV* pv;

		Vector3 size;

		float elasticity, friction;

		void* ud;

		bool collisionsEnabled();

		Render::Geometry* asGeometry();

		void destroyPrimitive();

		void modifySize(Vector3 size);

		void modifyShape(Shape shape);

		void modifyPosition(CoordinateFrame position);

		void modifyCollisions(bool canCollide);

		void modifyUserdata(void* data);
		void modifyGeomData(void* data);

		void modifyOffsetWorldCoordinateFrame(CoordinateFrame offset);

		void* getUserdata();

		CoordinateFrame getPosition();

		void setDisabled(bool disabled);

		void reset();

		void createPrimitive(RBX::Shape primitive, Vector3 size);

		void step();

		/* attached to a body: simulated */
		Primitive(Body* body);
		Primitive();
	};
}