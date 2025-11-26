#include "../Gury/Game/Services/service.h"
#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/World/runservice.h"

#include "Connectors/snap.h"
#include "Connectors/weld.h"
#include "Connectors/motor.h"
#include "Connectors/hinge.h"


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
	if (body0 && body1)
	{
		if (body0->body && body1->body)
		{
			return dAreConnectedExcluding(body0->body, body1->body, dJointTypeContact) || body0->body == body1->body;
		}
	}
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
	
	for (size_t i = 0; i < 6; i++)
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

	for (size_t i = 0; i < children->size(); i++)
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

void RBX::JointsService::snap(PartInstance* part1, PartInstance* part2)
{
	if (Builder::Collides(part1->primitive, part2->primitive))
	{
		bool shouldBeStatic = part1->anchored || part2->anchored;

		Connector* connector = fromLinkageAndPrimitives(Snapped, part1->primitive, part2->primitive, UNDEFINED);
		connector->build();

		OffsetBodyNode* currentSnapNode = new OffsetBodyNode();
		OffsetBodyNode* node0 = Builder::grabCurrentNode(part1->primitive);
		OffsetBodyNode* node1 = Builder::grabCurrentNode(part2->primitive);

		if (!node0 && !node1)
		{
			currentSnapNode = new OffsetBodyNode();
		}
		else if (node0 == node1)
		{
			currentSnapNode = node0;
		}

		if (currentSnapNode)
		{
			currentSnapNode->isStatic = shouldBeStatic;
			currentSnapNode->addPrimitive(part1->primitive);
			currentSnapNode->addPrimitive(part2->primitive);
			JointsService::get()->addConnectorNode(currentSnapNode);
		}

		JointsService::get()->addConnector(connector);

		if (RunService::get()->isRunning)
		{
			currentSnapNode->createBody();
		}
	}
}

bool RBX::JointsService::inStaticNode(Primitive* primitive)
{
	for (int i = 0; i < offsetBodyNodes.size(); i++)
	{
		OffsetBodyNode* node = offsetBodyNodes[i];
		if (node->isStatic)
		{
			if (node->primitives.contains(primitive))
			{
				return true;
			}
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

bool RBX::JointsService::arePrimitivesConnected(Primitive* primitive0, Primitive* primitive1)
{
	for (unsigned int i = 0; i < children->size(); i++)
	{
		Connector* connector = toInstance<Connector>(children->at(i));
		if (connector)
		{
			if (connector->prim0 == primitive0 && connector->prim1 == primitive1 || 
				connector->prim0 == primitive1 && connector->prim1 == primitive0)
			{
				return true;
			}
		}
	}
	return false;
}

void RBX::JointsService::addConnector(Connector* connector)
{
	if (connector)
	{
		if (!areConnectedUnbuiltPrimitives(connector->prim0, connector->prim1))
		{
			connector->setParent(this);
		}
	}
}

void RBX::JointsService::addConnectorNode(OffsetBodyNode* newNode)
{
	if (!offsetBodyNodes.contains(newNode))
	{
		offsetBodyNodes.append(newNode);
	}
}

void RBX::JointsService::removeConnectorNode(OffsetBodyNode* newNode)
{
	if (offsetBodyNodes.contains(newNode))
	{
		offsetBodyNodes.remove(offsetBodyNodes.findIndex(newNode));
	}
}

void RBX::JointsService::buildConnectors()
{

	/* build connectors */

	if (buildTime != NoBuild)
	{
		/* Link our connector nodes */
		linkNodes();

		/* clear out old bodies from world */

		for (int i = 0; i < oldBodies.size(); i++)
		{
			Body* body = oldBodies[i];
			if (body && body->body)
			{
				//body->destroyBody();
			}
		}

	}

}

void RBX::JointsService::buildGlobalJoints()
{
	if (buildTime != NoBuild)
	{
		Builder::buildGlobalJoints();
	}
}

void RBX::JointsService::linkNodes()
{
	for (int i = 0; i < offsetBodyNodes.size(); i++)
	{
		OffsetBodyNode* node = offsetBodyNodes[i];
		if (node)
		{
			linkNode(node);
		}
	}
}

void RBX::JointsService::linkNode(OffsetBodyNode* connectorNode)
{
	if (connectorNode)
	{
		connectorNode->createBody();
	}
}

RBX::JointsService* RBX::JointsService::get()
{
    return RBX::Datamodel::get()->jointService;
}