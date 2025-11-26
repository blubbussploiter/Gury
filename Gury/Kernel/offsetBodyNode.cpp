#include "jointsservice.h"
#include "../Gury/Game/Rendering/renderPrimitives.h"

CoordinateFrame getCenterOfMass(Array<RBX::Primitive*> primitives)
{
	CoordinateFrame center;
	Vector3 min, max;
	Matrix3 rot;

	for (unsigned int i = 0; i < primitives.size(); i++) /* modded version of this: https://devforum.roblox.com/t/how-do-i-get-the-middle-of-multiple-parts/2007200/20 */
	{
		RBX::Primitive* primitive = primitives[i];
		CoordinateFrame position = primitive->getPosition();
		Vector3 pos = position.translation;

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
			rot = position.rotation;
		}
	}
	center.translation = ((max - min) / 2) + min;
	center.rotation = rot;
	return center;
}

void RBX::OffsetBodyNode::addPrimitive(Primitive* primitive)
{
	if (!primitives.contains(primitive))
	{
		CoordinateFrame position = primitive->worldPosition;

		primitives.append(primitive);
		cofm = getCenterOfMass(primitives);

		if (thisBody)
		{
			if (primitive->isValid())
			{
				thisBody->attachPrimitive(primitive);
				primitive->body = thisBody;
				primitive->modifyOffsetWorldCoordinateFrame(position);
			}
			adjustCofm();
		}
	}
}

void RBX::OffsetBodyNode::removePrimitive(Primitive* primitive)
{
	if (primitives.contains(primitive))
	{
		primitives.fastRemove(primitives.findIndex(primitive));
		cofm = getCenterOfMass(primitives);

		if (thisBody)
		{
			if (primitive->isValid())
			{
				thisBody->detachPrimitive(primitive);
				primitive->body = 0; /* or old body.. maybe we should store these somewhere */
			}
			adjustCofm();
		}
	}
}

void RBX::OffsetBodyNode::doRender(RenderDevice* renderDevice)
{
	if (thisBody)
	{

		renderDevice->pushState();

		renderDevice->setObjectToWorldMatrix(thisBody->getPosition());

		renderDevice->setColor(Color3::RED);
		Primitives::drawBall(renderDevice, 2);
		renderDevice->setColor(Color3::WHITE);

		renderDevice->popState();

	}

	Extents extents = getExtents();
	Vector3 size = extents.size();

	Color3 color = Color3::green();
	if (isStatic)
	{
		color = Color3::red();
	}

	renderDevice->pushState();

	if (thisBody)
	{
		renderDevice->setObjectToWorldMatrix(thisBody->getPosition());
	}
	else {

		renderDevice->setObjectToWorldMatrix(cofm);

	}
	Primitives::drawOutline(renderDevice, -size, size, color, 0.25f);

	renderDevice->popState();

}

RBX::Extents RBX::OffsetBodyNode::getExtents()
{
	Vector3 min, max;

	for (int i = 0; i < primitives.size(); i++)
	{
		Primitive* primitive = primitives[i];
		if (primitive)
		{
			Extents extents;
			extents = primitive->getWorldExtents();

			if (min == Vector3::zero())
			{
				min = extents.low;
			}
			else
			{
				if (extents.low.x < min.x) min.x = extents.low.x;
				if (extents.low.y < min.y) min.y = extents.low.y;
				if (extents.low.z < min.z) min.z = extents.low.z;
			}

			if (max == Vector3::zero())
			{
				max = extents.high;
			}
			else
			{
				if (extents.high.x > max.x) max.x = extents.high.x;
				if (extents.high.y > max.y) max.y = extents.high.y;
				if (extents.high.z > max.z) max.z = extents.high.z;
			}
		}
	}
	if (min == Vector3::zero() && max == Vector3::zero()) {
		return Extents();
	}
	else {
		return Extents(min / 2, max / 2);
	}
}

void RBX::OffsetBodyNode::adjustCofm()
{
	if (thisBody)
	{
		dMass mass;

		float rotation[12] = toDMatrix3(cofm.rotation);

		dBodyGetMass(thisBody->body, &mass);

		mass.rotate(rotation);
		mass.translate(cofm.translation.x, cofm.translation.y, cofm.translation.z);

		thisBody->modifyMass(mass);
	}
}

void RBX::OffsetBodyNode::createBody()
{
		if (primitives.size() > 0)
		{
			if (!isStatic)
			{
				if (!thisBody)
				{
					dMass mass;

					float rotation[12] = toDMatrix3(cofm.rotation);

					thisBody = new Body();
					thisBody->createBody(Vector3::one());

					dBodyGetMass(thisBody->body, &mass);

					mass.rotate(rotation);
					mass.translate(cofm.translation.x, cofm.translation.y, cofm.translation.z);

					thisBody->modifyPosition(cofm);

					for (int i = 0; i < primitives.size(); i++)
					{
						Primitive* primitive = primitives[i];
						if (primitive)
						{
							if (primitive->isValid())
							{
								primitive->body = thisBody;
								thisBody->attachPrimitive(primitive);
								primitive->modifyOffsetWorldCoordinateFrame(primitive->worldPosition);
							}
						}
					}

					thisBody->modifyMass(mass);
				}
			}
			else /* No need for a body... no nothin' */
			{
				for (int i = 0; i < primitives.size(); i++)
				{
					Primitive* primitive = primitives[i];
					if (primitive)
					{
						if (primitive->isValid())
						{
							if (primitive->body)
							{
								primitive->body->detachPrimitive(primitive);
								primitive->body->destroyBody();
								primitive->body = 0;
							}
						}
					}
				}
			}
		}
}