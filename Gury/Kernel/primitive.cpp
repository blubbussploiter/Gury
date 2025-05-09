
#include "kernel.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/World/camera.h"

#include "primitive.h"


bool RBX::Primitive::collisionsEnabled()
{
	return dGeomIsEnabled(geom[0]);
}

RBX::Render::Geometry* RBX::Primitive::asGeometry()
{
	return new Render::Geometry(pv->position, size, shape);
}

void RBX::Primitive::destroyPrimitive()
{
	if (geom[0])
	{
		dGeomDestroy(geom[0]);
		geom[0] = nullptr;
	}
}

void RBX::Primitive::modifySize(Vector3 size)
{
	if (geom[0])
	{
		this->size = size;
		switch (shape)
		{
			case Cylinder:
			{
				dGeomSphereSetRadius(geom[0], size.y / 2);
				break;
			}
			case Ball:
			{
				dGeomSphereSetRadius(geom[0], size.y / 2);
				break;
			}
			case Block:
			{
				dGeomBoxSetLengths(geom[0], size.x * 2, size.y * 2, size.z * 2);
				break;
			}
		}
	}

}

void RBX::Primitive::modifyShape(Shape shape)
{
	this->shape = shape;
	reset();
}

void RBX::Primitive::modifyPosition(CoordinateFrame position)
{
	if (geom[0])
	{

		Vector3 translation = position.translation;
		Matrix3 rotation = position.rotation;

		float dRotation[12] = { rotation[0][0], rotation[0][1], rotation[0][2], 0, rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 };

		dGeomSetPosition(geom[0], translation.x, translation.y, translation.z);
		dGeomSetRotation(geom[0], dRotation);

		onPVChanged(this);
	}
}

void RBX::Primitive::modifyCollisions(bool canCollide)
{
	if (!geom[0]) return;
	if (!canCollide)
	{
		dGeomDisable(geom[0]);
	}
	else
	{
		dGeomEnable(geom[0]);
	}
}

void RBX::Primitive::modifyUserdata(void* data)
{
	ud = data;
}

void RBX::Primitive::modifyGeomData(void* data)
{
	if (geom[0])
	{
		dGeomSetData(geom[0], data);
	}
}

void* RBX::Primitive::getUserdata()
{
	return ud;
}

CoordinateFrame RBX::Primitive::getPosition()
{
	const dReal* position;
	const dReal* rotation;

	position = dGeomGetPosition(geom[0]);
	rotation = dGeomGetRotation(geom[0]);

	return CoordinateFrame(Matrix3(rotation[0], rotation[1], rotation[2],
		rotation[4], rotation[5], rotation[6],
		rotation[8], rotation[9], rotation[10]),
		Vector3(position[0], position[1], position[2]));
}

void RBX::Primitive::modifyOffsetWorldCoordinateFrame(CoordinateFrame offset)
{
	if (!geom[0]) return;

	Vector3 position = offset.translation;
	Matrix3 rotation = offset.rotation;

	float dRotation[12] = { rotation[0][0], rotation[0][1], rotation[0][2], 0, rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 };

	dGeomSetOffsetWorldPosition(geom[0], position.x, position.y, position.z);
	dGeomSetOffsetWorldRotation(geom[0], dRotation);
}

void RBX::Primitive::setDisabled(bool disabled)
{
	modifyCollisions(!disabled);
}

void RBX::Primitive::reset()
{
	if (!geom[0]) return;

	dGeomDestroy(geom[0]);
	geom[0] = 0;

	createPrimitive(shape, size);

	if (body)
	{
		body->attachPrimitive(this);
	}
}

void RBX::Primitive::createPrimitive(RBX::Shape primitive, Vector3 size)
{
	Vector3 position = pv->position.translation;
	Matrix3 rotation = pv->position.rotation;

	if (geom[0]) return;

	this->shape = primitive;
	this->size = size;

	switch (shape)
	{
	case Cylinder:
	{
		geom[0] = dCreateSphere(Gurnel::get()->space, size.y);
		break;
	}
	case Ball:
	{
		geom[0] = dCreateSphere(Gurnel::get()->space, size.y);
		break;
	}
	default:
	{
		geom[0] = dCreateBox(Gurnel::get()->space, size.x * 2, size.y * 2, size.z * 2);
		break;
	}
	}

	float dRotation[12] = { rotation[0][0], rotation[0][1], rotation[0][2], 0, rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 };

	dGeomSetPosition(geom[0], position.x, position.y, position.z);
	dGeomSetRotation(geom[0], dRotation);

	dGeomSetData(geom[0], this);

}

void RBX::Primitive::step()
{

	if (!geom[0])
	{
		return;
	}

	if (body)
	{
		if (!body->disabled)
		{
			body->step();
			pv->velocity = body->pv->velocity;
		}
	}

	pv->position = getPosition();
	onPVChanged(this);

}

RBX::Primitive::Primitive(Body* body)
{
	this->body = body;
	this->pv = body->pv;
	elasticity = 0.5f;
	friction = 0.3f;
}

RBX::Primitive::Primitive()
{
	this->pv = new PV();
	this->body = 0;
	elasticity = 0.5f;
	friction = 0.3f;
}