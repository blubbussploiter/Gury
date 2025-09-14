#include "../Gury/Game/Services/service.h"

#include "Connectors/snap.h"
#include "Connectors/weld.h"
#include "Connectors/motor.h"
#include "Connectors/hinge.h"

#include "../Gury/Game/Rendering/renderScene.h"

RTTR_REGISTRATION
{
	rttr::registration::enumeration<RBX::BuildTime>("BuildTime")
	(
				rttr::value("Build", RBX::Build),
				rttr::value("NoBuild", RBX::NoBuild)
	);

	rttr::registration::class_<RBX::JointsService>("JointsService")
		 .constructor<>()
		 .property("buildTime", &RBX::JointsService::getBuildTime, &RBX::JointsService::setBuildTime)(rttr::metadata("Type", RBX::Secret));
}


RBX::Connector* RBX::JointsService::fromLinkageAndPrimitives(Linkage linkage, Primitive* prim0, Primitive* prim1, NormalId surface)
{
	switch (linkage)
	{
	case Motored:
	{
		return new MotorConnector(prim0, prim1, surface);
	}
	case Hinged:
	{
		return new HingeConnector(prim0, prim1, surface);
	}
	case Welded:
	{
		return new WeldConnector(prim0, prim1, surface);
	}
	case Glued:
	case Snapped:
	{
		return new SnapConnector(prim0, prim1, surface);
	}
	}
	return 0;
}

bool RBX::JointsService::areConnectedPrimitives(Primitive* prim0, Primitive* prim1)
{
	if (!prim0->geom[0] || !prim1->geom[0]) return false;
	if (!bodyIsConnector(prim0->body) || !bodyIsConnector(prim1->body)) return false;
	return dGeomGetBody(prim0->geom[0]) == dGeomGetBody(prim1->geom[0]);
}

bool RBX::JointsService::areConnectedBodies(Body* body0, Body* body1)
{
	if (!body0->body || !body1->body) return false;
	return dAreConnectedExcluding(body0->body, body1->body, dJointTypeContact);
}

bool RBX::JointsService::bodyIsConnector(Body* body)
{
	if (!body) return 0;
	Connector* data = (Connector*)body->getUserdata();
	return data != 0;
}

RBX::Linkage RBX::JointsService::makeLinkage(SurfaceType s0, SurfaceType s1)
{
	if ((s0 == Studs || s1 == Studs) && (s0 == Inlet || s1 == Inlet)) return Snapped;
	if (s0 == Motor || s1 == Motor) return Motored;
	if (s0 == Hinge || s1 == Hinge) return Hinged;
	if (s0 == Weld || s1 == Weld) return Welded;
	if (s0 == Glue || s1 == Glue) return Glued;
	return NotLinked;
}

RBX::NormalId RBX::JointsService::fromNormal(PVInstance* object, Vector3 normal)
{
	float tolerance = 0.001f;
	
	for (unsigned int i = 0; i < 6; i++)
	{
		NormalId n = normals[i];
		if (dot(getNormalFromNormalId(object, n), normal) > tolerance)
		{
			return n;
		}
	}
	return UNDEFINED;
}

bool RBX::JointsService::areConnectedUnbuiltPrimitives(Primitive* prim0, Primitive* prim1)
{

	/* before build(), compare prim0 and prim1 */

	for (unsigned int i = 0; i < children->size(); i++)
	{
		Connector* connector = toInstance<Connector>(children->at(i));
		if (connector)
		{
			if (connector->prim0 == prim0 && connector->prim1 == prim1 ||
				connector->prim0 == prim1 && connector->prim1 == prim0)
				return true;
		}
	}

	return false;

}

RBX::Connector* RBX::JointsService::getConnecting(Primitive* primitive)
{
	for (unsigned int i = 0; i < children->size(); i++)
	{
		Connector* connector = toInstance<Connector>(children->at(i));
		if (connector)
		{
			if (connector->prim0 == primitive || connector->prim1 == primitive)
			{
				return connector;
			}
		}
	}
	return 0;
}

void RBX::JointsService::addConnector(Connector* connector)
{
	if (!connector) return;
	if (areConnectedUnbuiltPrimitives(connector->prim0, connector->prim1)) return;
	connector->setParent(this);
}

void RBX::JointsService::buildConnectors()
{
	/* build connectors */

	if (buildTime != NoBuild)
	{
		int connectorsBuilt = 0;

		for (int i = 0; i < children->size(); i++)
		{
			Connector* connector = toInstance<Connector>(children->at(i));
			if (connector && !connector->connected())
			{
				connector->link();
				connectorsBuilt++;
			}
		}

		/* clear out old bodies from world */

		for (int i = 0; i < old_Bodies.size(); i++)
		{
			Body* body = old_Bodies[i];
			if (body && body->body)
			{
				body->destroyBody();
			}
		}

		RBX::StandardOut::print(RBX::MESSAGE_INFO, "JointsService::buildConnectors() finalized, %d connectors, %d primitives and %d bodies in world", connectorsBuilt, Gurnel::get()->getPrimitivesInWorld(), Gurnel::get()->getBodiesInWorld());

	}
}

void RBX::JointsService::buildGlobalJoints()
{
	if (buildTime != NoBuild)
	{
		Experiment::buildGlobalJoints();
	}
}

void RBX::JointsService::doLink(Connector* connector)
{
	RBX::StandardOut::print(RBX::MESSAGE_INFO, "master connector = 0x%08X", connector->master);
}

RBX::JointsService* RBX::JointsService::get()
{
    return RBX::Datamodel::get()->jointService;
}