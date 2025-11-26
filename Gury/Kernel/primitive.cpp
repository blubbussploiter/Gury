
#include "kernel.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/World/camera.h"

#include "primitive.h"
#include "jointsservice.h"


bool RBX::Primitive::collisionsEnabled()
{
	return dGeomIsEnabled(geom[0]);
}

bool RBX::Primitive::isValid() const
{
	return geom[0] != nullptr;
}

RBX::Render::Geometry* RBX::Primitive::asGeometry()
{
	return new Render::Geometry(worldPosition, size, shape);
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
	this->size = size;

	if (geom[0])
	{
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
	this->worldPosition = position;

	if (geom[0])
	{
		Vector3 translation = position.translation;
		Matrix3 rotation = position.rotation;

		float dRotation[12] = { rotation[0][0], rotation[0][1], rotation[0][2], 0, rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 };

		dGeomSetPosition(geom[0], translation.x, translation.y, translation.z);
		dGeomSetRotation(geom[0], dRotation);

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

void RBX::Primitive::attachLink(Connector* connector)
{
	if (links.contains(connector))
	{
		return;
	}
	links.push_back(connector);
}

void RBX::Primitive::detachLink(Connector* connector)
{
	if (!links.contains(connector))
	{
		return;
	}
	links.push_back(connector);
}

void* RBX::Primitive::getUserdata()
{
	return ud;
}

CoordinateFrame RBX::Primitive::getPosition()
{
	if (geom[0])
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
	else
	{
		return worldPosition;
	}
}

void RBX::Primitive::modifyOffsetWorldCoordinateFrame(CoordinateFrame offset)
{
	if (body)
	{

		if (!geom[0]) return;

		Vector3 position = offset.translation;
		Matrix3 rotation = offset.rotation;

		float dRotation[12] = { rotation[0][0], rotation[0][1], rotation[0][2], 0, rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 };

		if (dGeomGetBody(geom[0]))
		{
			dGeomSetOffsetWorldPosition(geom[0], position.x, position.y, position.z);
			dGeomSetOffsetWorldRotation(geom[0], dRotation);
		}

	}
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

	if (!geom[0])
	{
		Vector3 position = worldPosition.translation;
		Matrix3 rotation = worldPosition.rotation;

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

}

void RBX::Primitive::step()
{
	if (geom[0])
	{
		if (body)
		{
			if (!body->disabled)
			{
				body->step();
			}

			CoordinateFrame position = getPosition();

			if (!body->atRest() || body->wantsReset)
			{
				if (physicalInstance)
				{
					if (body->wantsReset)
					{
						physicalInstance->step();
						body->wantsReset = false;
					}
					else
					{
						if (position != worldPosition)
						{
							worldPosition = position;
							physicalInstance->step();
						}
					}
				}
			}
		}
	}
}

RBX::Primitive::Primitive(Body* body)
{
	this->body = body;
	/* independence is better */
	elasticity = 0.5f;
	friction = 0.3f;
}

RBX::Primitive::Primitive()
{
	this->body = 0;
	elasticity = 0.5f;
	friction = 0.3f;
}