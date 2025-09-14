#include "snap.h"
#include "../Gury/Game/Services/stdout.h"

RBX::SnapConnector* RBX::SnapConnector::getConnectingSnap(Primitive* prim)
{
	if (!prim->body) return 0;

	void* rawData = prim->getUserdata();
	if (!rawData) return 0;

	RBX::Instance* instance = static_cast<Instance*>(rawData);

	if (RBX::IsA<SnapConnector>(instance))
		return (SnapConnector*)instance;

	return 0;
}

bool isASnapConnector(RBX::Primitive* prim)
{
	return (RBX::SnapConnector::getConnectingSnap(prim) != 0);
}

CoordinateFrame getCenterOfMass(Array<RBX::Primitive*>* primitives)
{
	CoordinateFrame center;
	Vector3 min, max;
	Matrix3 rot;

	for (unsigned int i = 0; i < primitives->size(); i++) /* modded version of this: https://devforum.roblox.com/t/how-do-i-get-the-middle-of-multiple-parts/2007200/20 */
	{
		RBX::Primitive* primitive = (*primitives)[i];
		Vector3 pos = primitive->getPosition().translation;

		if (min == Vector3::zero())
		{
			min = pos;
		}
		else
		{
			if (pos.x < min.x) min.x = pos.x;
			if (pos.y < min.y) min.y = pos.y;
			if (pos.z < min.z) min.z = pos.z;
		}

		if (max == Vector3::zero())
		{
			max = pos;
		}
		else
		{
			if (pos.x > max.x) max.x = pos.x;
			if (pos.y > max.y) max.y = pos.y;
			if (pos.z > max.z) max.z = pos.z;
			rot = primitive->getPosition().rotation;
		}
	}
	center.translation = ((max - min) / 2) + min;
	center.rotation = rot;
	return center;
}

void RBX::SnapConnector::build()
{
	Vector3 offsetPosition;
	Matrix3 offsetRotation;

	JointsService* jointsService;
	jointsService = JointsService::get();

	if (!prim0->geom[0] || !prim1->geom[0]) return;

	SnapConnector* connector0, * connector1;
	connector0 = getConnectingSnap(prim0);
	connector1 = getConnectingSnap(prim1);

	if (connector0)
	{
		/* 9-14-25 this is where i left off (brickenhoff) */
	}
	if (connector1)
	{

	}

	prim0->modifyUserdata(this);
	prim1->modifyUserdata(this);

	primitives->push_back(prim0);
	primitives->push_back(prim1);

	anchored = (prim0->body != 0 && prim1->body != 0);

}

void RBX::SnapConnector::link()
{
	/* do like JointService::doLink(Connector* connector) */
	JointsService::get()->doLink(this);
}


void RBX::SnapConnector::unlink()
{
	/* do like JointService::doUnlink(Connector* connector) */
}
