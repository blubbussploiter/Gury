#include "snap.h"
#include "../Gury/Game/Services/stdout.h"

RBX::SnapConnector* RBX::SnapConnector::getConnectingSnap(Primitive* prim)
{
	if (!prim->body) return 0;

	void* rawData = prim->body->getUserdata();
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

void copyArray(G3D::Array<RBX::Primitive*>*& array0, G3D::Array<RBX::Primitive*>*& array1)
{
	for (int i = 0; i < array0->size(); i++)
	{
		RBX::Primitive* primitive = (*array0)[i];
		if (primitive && 
			!array1->contains(primitive))
		{
			array1->push_back(primitive);
		}
	}
	array0->fastClear();
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

	body0 = prim0->body;
	body1 = prim1->body;

	SnapConnector* connector0, * connector1;
	connector0 = getConnectingSnap(prim0);
	connector1 = getConnectingSnap(prim1);

	if (connector0)
	{
		if (!connector)
		{
			connector = connector0->connector;
		}
		copyArray(connector0->primitives, primitives);
	}
	if (connector1)
	{
		if (!connector)
		{
			connector = connector1->connector;
		}
		copyArray(connector1->primitives, primitives);
	}

	if (!connector)
	{
		connector = new Body();
		connector->createBody(Vector3(1, 1, 1));

		if (body0 && body1)
		{
			CoordinateFrame pos0, pos1, pos;
			Vector3 t0, t1;
			Matrix3 m0, m1;

			pos0 = prim0->pv->position;
			pos1 = prim1->pv->position;

			t0 = pos0.translation;
			t1 = pos1.translation;
			m0 = pos0.rotation;
			m1 = pos1.rotation;

			pos = (t0 + t1) / 2;
			pos.rotation = m0 * m1;
			
			connector->modifyPosition(pos);
		}
	}

	primitives->push_back(prim0);
	primitives->push_back(prim1);

	connector->modifyUserdata(this);

	bool notAnchored = (prim0->body != 0 && prim1->body != 0);

	if (connector)
	{
		CoordinateFrame cofm;

		for (int i = 0; i < primitives->size(); i++)
		{
			Primitive* prim = (*primitives)[i];
			
			if (notAnchored)
			{
				connector->attachPrimitive(prim);
				prim->modifyOffsetWorldCoordinateFrame(prim->pv->position);
			}
			else
			{
				if (prim->body)
				{
					if (!isASnapConnector(prim))
					{
						jointsService->old_Bodies.push_back(prim->body);
					}
				}
				connector->detachPrimitive(prim);
				connector->destroyBody();
			}

		}
		cofm = getCenterOfMass(primitives);

		if (connector->body)
		{
			dMass mass;
			float rotation[12] = toDMatrix3(cofm.rotation);

			dBodyGetMass(connector->body, &mass);

			mass.rotate(rotation);
			mass.translate(cofm.translation.x, cofm.translation.y, cofm.translation.z);
			mass.adjust(1.3f * primitives->size());

			connector->modifyMass(mass);
		}
	}

}

void RBX::SnapConnector::unlink()
{
	if (!connector || !prim0->geom[0] || !prim1->geom[0]) return;

	connector->detachPrimitive(prim0);
	connector->detachPrimitive(prim1);
	connector->destroyBody();

	dGeomClearOffset(prim0->geom[0]);
	dGeomClearOffset(prim1->geom[0]);

	if (body0)
	{
		body0->setDisabled(0);
		body0->attachPrimitive(prim0);
	}

	if (body1)
	{
		body1->setDisabled(0);
		body1->attachPrimitive(prim1);
	}
}
