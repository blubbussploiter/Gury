#pragma once

#include <G3DAll.h>
#include <vector>

#include "../Gury/Kernel/kernel.h"
#include "../Gury/Kernel/primitive.h"

#include "../Gury/Game/Objects/decal.h"
#include "../Gury/Game/Rendering/brickcolor.h"

#include "../Gury/Game/World/extents.h"

#include "../Gury/Game/Objects/IRenderable.h"
#include "../Gury/Game/Objects/ISelectable.h"

#include "../Gury/Game/Rendering/surfaceFactory.h"
#include "../Gury/Game/Rendering/textureReserve.h"
#include "../Gury/Game/Rendering/worldManager.h"

#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/rbxmath.h"

namespace RBX
{

	class PVInstance :
		public Render::IRenderable, public RBX::ISelectable
	{
	private:

		float elasticity, friction, erp, cfm;
		PV* startPV;

	public:

		PV* pv;
		Primitive* primitive;

	public:

		void resetPV()
		{
			pv->position = startPV->position;
			pv->velocity = startPV->velocity;
			primitive->modifyPosition(pv->position);
			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
		}

		void savePV()
		{
			if (!startPV->position.isIdentity())
			{
				return;
			}
			startPV->position = getCFrame();
			startPV->velocity.linear = getVelocity();
			startPV->velocity.rotational = getRotVelocity();
		}

		Body* getBody()
		{
			if (primitive->body) return primitive->body;
			return 0;
		}

		Primitive* getPrimitive()
		{
			return primitive;
		}

		void setVelocity(Vector3 newVelocity)
		{
			pv->velocity.linear = newVelocity;

			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
			onChanged(this, getPropertyByName("Velocity"));
		}

		void setRotVelocity(Vector3 newVelocity)
		{
			pv->velocity.rotational = newVelocity;

			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
			onChanged(this, getPropertyByName("RotVelocity"));
		}


		void setCenter(CoordinateFrame c)
		{
			setCFrame(c);
		}

		Vector3 getPosition() { return getCoordinateFrame().translation; }

		void setPosition(Vector3 p)
		{
			setCFrame(CoordinateFrame(pv->position.rotation, p));
		}

		CoordinateFrame getCFrame() {
			if (primitive)
			{
				return primitive->getPosition();
			}
			return CoordinateFrame();
		}

		void setCFrame(CoordinateFrame cf)
		{
			pv->position = cf;
			primitive->modifyPosition(pv->position);
			if (primitive->body)
			{
				primitive->body->modifyPosition(pv->position);
			}
			edit();
		}

		bool isConnected() const
		{
			if (primitive)
			{
				return (primitive->links.size() > 0); /* or primitives.links.size(), but that would be ugly. */
			}
			return false;
		}
		bool isDirectlyConnectedTo(Connector* connector) const
		{
			if (primitive)
			{
				return (primitive->links.contains(connector)); 
			}
			return false;
		}

		CoordinateFrame getCoordinateFrame() { return getCFrame(); }

		Vector3 getVelocity() { return pv->velocity.linear; }
		Vector3 getRotVelocity() { return pv->velocity.rotational; }

		Vector3 getEulerAngles()
		{
			return Math::getEulerAngles(pv->position.rotation);
		}

		void lookAt(Vector3 v)
		{
			CoordinateFrame cf;
			cf = getCFrame();
			cf.lookAt(v);
			setCFrame(cf);
		}

		CoordinateFrame getCenter()
		{
			return pv->position;
		}


		float getElasticity() { return elasticity; }
		void setElasticity(float el) { elasticity = el; }
		float getFriction() { return friction; }
		void setFriction(float f) { friction = f; }
		float getERP() { return erp; }
		float getCFM() { return cfm; }
		void setCFM(float _cfm) { cfm = _cfm; }
		void setERP(float _erp) { erp = _erp; }

		void RemovePhysicalPresence();

		virtual void CreatePhysicalPresence() {}
		virtual void step() {}

		~PVInstance();
		PVInstance();

		RTTR_ENABLE(RBX::Render::IRenderable)
	};

}