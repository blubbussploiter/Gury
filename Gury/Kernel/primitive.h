#pragma once

#include <G3DAll.h>
#include "ode/ode.h"

#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Reflection/signal.h"

#include "../Gury/Game/Objects/geometry.h"
#include "../Gury/Game/World/extents.h"

#include "body.h"

namespace RBX
{
	class Connector;

	class Primitive
	{
	public:
		PVInstance* physicalInstance; /* Our PVInstance, differentiated from UD now, since we also tend to store our connectors in UD. */
		Array<Connector*> links;
		RBX::Shape shape;

		dGeomID geom[1];

		Body* body;
		CoordinateFrame worldPosition; /* current Primitive location, DO NOT USE PV ANYMORE */

		Vector3 size;

		float elasticity, friction;

		void* ud;

		bool collisionsEnabled();

		bool isValid() const;

		Render::Geometry* asGeometry();

		void destroyPrimitive();

		void modifySize(Vector3 size);

		void modifyShape(Shape shape);

		void modifyPosition(CoordinateFrame position);

		void modifyCollisions(bool canCollide);

		void modifyUserdata(void* data);
		void modifyGeomData(void* data);

		void attachLink(Connector* connector);
		void detachLink(Connector* connector);

		void modifyOffsetWorldCoordinateFrame(CoordinateFrame offset);

		Extents getLocalExtents() const { return Extents(-size, size); }

		Extents getWorldExtents()
		{
			Extents localExtents = getLocalExtents();
			return localExtents.toWorldSpace(getPosition());
		}

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