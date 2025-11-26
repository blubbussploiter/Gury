#include "../Gury/Kernel/jointsservice.h"
#include "../Gury/Game/Objects/instance.h"

bool RBX::Connector::diagPrimitivesAreTouching()
{
	dGeomID geom0 = prim0->geom[0], geom1 = prim1->geom[0];

	return true;
}

void RBX::Connector::diagRender(RenderDevice* rd)
{
	Vector3 interceptPos;
	Vector3 v_p0, v_p1;

	rd->setObjectToWorldMatrix(CoordinateFrame());

	interceptPos = getInterceptPosition().translation;
	v_p0 = prim0->pv->position.translation;
	v_p1 = prim1->pv->position.translation;

	Sphere s_p0, s_p1;

	s_p0.radius = 0.2f;
	s_p0.center = v_p0;
	s_p1.radius = 0.2f;
	s_p1.center = v_p1;

	Color3 color;
	color = (prim0->body == 0 || prim1->body == 0) ? Color3::blue() : Color3::red();

	Draw::lineSegment(LineSegment::fromTwoPoints(v_p0, v_p1), rd, color, 0.5f);

	Draw::sphere(s_p0, rd, color, Color4::clear());
	Draw::sphere(s_p1, rd, color, Color4::clear());

	

}

void RBX::Connector::diagRenderPrimitiveOutlines(RenderDevice* rd)
{
	Color3 color;
	color = (prim0->body == 0 || prim1->body == 0) ? Color3::blue() : Color3::red();
	rd->setObjectToWorldMatrix(prim0->pv->position);
	Primitives::drawOutline(rd, -prim0->size, prim0->size, color, 0.1f);
	rd->setObjectToWorldMatrix(prim1->pv->position);
	Primitives::drawOutline(rd, -prim1->size, prim1->size, color, 0.1f);
}

CoordinateFrame RBX::Connector::getInterceptPosition()
{
	return (prim0->pv->position.translation + prim1->pv->position.translation) / 2;
}

bool RBX::Connector::connected()
{
	if (JointsService::areConnectedPrimitives(prim0, prim1)) return 1;
	if (prim0->body && prim1->body)
	{
		return JointsService::areConnectedBodies(prim0->body, prim1->body);
	}
	return 0;
}

RBX::Connector* RBX::Connector::getConnectingConnector(RBX::Primitive* prim)
{
	if (prim->body)
	{

		void* rawData = prim->getUserdata();
		if (rawData)
		{
			RBX::Instance* instance = static_cast<Instance*>(rawData);

			if (RBX::IsA<Connector>(instance))
				return (Connector*)instance;

		}
	}
	return 0;
}

RBX::ConnectorNode* RBX::Connector::getParentNode(RBX::Primitive* prim0, RBX::Primitive* prim1)
{
	Array<Connector*> connectorArray;

	/* get from array recursively (hopefully) */
	getConnectingConnectorsByPrim(prim0, connectorArray);
	getConnectingConnectorsByPrim(prim1, connectorArray);

	for (int i = 0; i < connectorArray.size(); i++)
	{
		Connector* connector = connectorArray[i];
		if (connector)
		{
			if (connector->parentNode)
			{
				return connector->parentNode;
			}
		}
	}

	return 0;
}

void RBX::Connector::getConnectingConnectorsByPrim(RBX::Primitive* prim, Array<Connector*>& finalArray)
{
	Connector* connectingPrim = getConnectingConnector(prim);

	if (connectingPrim)
	{
		getConnectingConnectorsByConnector(connectingPrim, finalArray);
	}
}

void RBX::Connector::getConnectingConnectorsByConnector(RBX::Connector* connector, Array<Connector*>& finalArray)
{
	Primitive* prim0 = connector->prim0;
	Primitive* prim1 = connector->prim1;

	if (prim0 && prim1)
	{
		Connector* connectingPrim0 = getConnectingConnector(prim0);
		Connector* connectingPrim1 = getConnectingConnector(prim1);

		if (connectingPrim0)
		{
			if (!finalArray.contains(connectingPrim0))
			{
				finalArray.push_back(connectingPrim0);
			}
		}

		if (connectingPrim1)
		{
			if (!finalArray.contains(connectingPrim1))
			{
				finalArray.push_back(connectingPrim1);
			}
		}
	}
}

void RBX::Connector::build()
{

	if (prim0 && prim1)
	{

		ConnectorNode* parentNode = getParentNode(prim0, prim1);

	}

}