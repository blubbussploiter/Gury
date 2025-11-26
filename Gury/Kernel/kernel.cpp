
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Rendering/renderScene.h"
#include "../Game/globalSounds.h"

#include "kernel.h"
#include "primitive.h"

/* fix this bunk ass physics engine someday */

void RBX::Gurnel::addPrimitive(Primitive* primitive)
{
	if (!primitives.contains(primitive))
	{
		primitives.append(primitive);
	}
}

void RBX::Gurnel::removePrimitive(Primitive* primitive)
{
	//primitive->destroyPrimitive();
	//objects.remove(objects.findIndex(primitive));
	if (!garbagePrimitiveQueue.contains(primitive))
	{
		garbagePrimitiveQueue.append(primitive);
	}
}

void RBX::Gurnel::collectGarbage()
{

	for (int i = 0; i < garbagePrimitiveQueue.size(); i++)
	{
		Primitive* primitive = garbagePrimitiveQueue[i];
		if (primitive)
		{
			PVInstance* physInstance = primitive->physicalInstance;
			if (physInstance)
			{
				physInstance->remove();
			}
			primitive->destroyPrimitive();
			if (primitive->body)
			{
				primitive->body->doDestroy();
				primitive->body = 0;
			}
			primitives.remove(primitives.findIndex(primitive));
			//delete primitive;
		}
	}
	for (int i = 0; i < garbageBodyQueue.size(); i++)
	{
		Body* body = garbageBodyQueue[i];
		if (body)
		{
			body->doDestroy();
			//delete body;
		}
	}
	if (garbagePrimitiveQueue.size() > 0)
	{
		garbagePrimitiveQueue.clear();
		garbageBodyQueue.clear();

	}
}

void RBX::Gurnel::haltKernel()
{
	for (int i = 0; i < primitives.size(); i++)
	{
		primitives[i]->destroyPrimitive();
	}
	collectGarbage();
}

void RBX::Gurnel::collisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	int i;

	Primitive* prim0 = (Primitive*)dGeomGetData(o1);
	Primitive* prim1 = (Primitive*)dGeomGetData(o2);

	if (prim0 && prim1)
	{


		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);

		if (b1 && b2 && dAreConnected(b1, b2))
		{
			return;
		}

		dContact contact[4]{};
		int numc = dCollide(o1, o2, 4, &contact[0].geom, sizeof(dContact));

		if (numc > 0)
		{
			for (i = 0; i < numc; i++)
			{

				contact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;

				// Define contact surface properties

				float fric = std::max<float>(prim0->friction, prim1->friction);
				float elas = std::max<float>(prim0->elasticity, prim1->elasticity);

				contact[i].surface.bounce = elas; //Elasticity
				contact[i].surface.mu = fric; //Friction
				contact[i].surface.slip1 = 0.0f;
				contact[i].surface.slip2 = 0.0f;
				contact[i].surface.soft_erp = 0.8f;
				contact[i].surface.soft_cfm = 2.0f;

				dJointID c = dJointCreateContact(
					Gurnel::get()->world,
					Gurnel::get()->contacts,
					&contact[i]);

				dJointAttach(c, b1, b2);
			}

		}

	}

}


void RBX::Gurnel::step(float stepInS, int iterations)
{
	/* ode stuff, step the simulation */

	dJointGroupEmpty(contacts);
	dSpaceCollide(space, 0, &Gurnel::collisionCallback);
	dWorldStepFast1(world, stepInS, iterations);

}

void RBX::Gurnel::afterStep()
{
	/* gury stuff, create them or step them */

	collectGarbage();
	spawnWorld();
	
	/* step */

	for (int i = 0; i < primitives.size(); i++)
	{
		Primitive* prim = primitives[i];
		if (prim)
		{
			if (!outOfBoundCheck(prim))
			{
				prim->step();
			}
			else
			{
				removePrimitive(prim);
			}
		}
	}

}

void RBX::Gurnel::markForReset(bool shouldReset)
{
	for (int i = 0; i < primitives.size(); i++)
	{
		Primitive* prim = primitives[i];
		if (prim->body)
		{
			prim->body->wantsReset = shouldReset;
		}
	}
}

void RBX::Gurnel::spawnWorld()
{
	/* not going by very inconsistent primitives anymore, going by pvinstances in scene */

	RBX::Instances* instances = WorldScene::get()->getArrayOfObjects();

	for (size_t i = 0; i < instances->size(); i++)
	{
		PVInstance* pv = toInstance<PVInstance>(instances->at(i));
		if (pv)
		{
			pv->CreatePhysicalPresence();
		}
	}
}

bool RBX::Gurnel::outOfBoundCheck(Primitive* object)
{
	if (object->worldPosition.translation.y <= -worldLimitsY)
	{
		return 1;
	}
	return 0;
}

int RBX::Gurnel::getPrimitivesInWorld()
{
	int hasGeom = 0;
	for (unsigned int i = 0; i < primitives.size(); i++)
	{
		Primitive* primitive = primitives[i];
		if (primitive && primitive->geom[0])
		{
			hasGeom++;
		}
	}
	return hasGeom;
}

int RBX::Gurnel::getBodiesInWorld()
{
	int hasAttachedBody = 0;
	for (unsigned int i = 0; i < primitives.size(); i++)
	{
		Primitive* primitive = primitives[i];
		if (primitive && primitive->body && primitive->body->body)
		{
			hasAttachedBody++;
		}
	}
	return hasAttachedBody;
}

float RBX::Gurnel::getGravity()
{
	dVector3 gravity;
	dWorldGetGravity(world, gravity);
	return gravity[1];
}

void RBX::Gurnel::close()
{
	RBX::StandardOut::print(RBX::MESSAGE_INFO, "Gurnel::cleanup() closing Gurnel");
	dCloseODE();
	dJointGroupDestroy(contacts);
	dSpaceDestroy(space);
	dWorldDestroy(world);
}

void RBX::Gurnel::diag_renderObjects(RenderDevice* rd)
{
	float radius = 0.5f;
	Color3 color = Color3::green();

	rd->pushState();

	for (unsigned int i = 0; i < primitives.size(); i++)
	{
		Primitive* primitive = primitives[i];
		if (primitive)
		{
			if (primitive->body && primitive->body->body)
			{
				const dReal* center = dBodyGetPosition(primitive->body->body);
				CoordinateFrame cframe = Vector3(center[0], center[1], center[2]);
				rd->setObjectToWorldMatrix(cframe);
				Draw::sphere(Sphere(Vector3::zero(), 0.5f), rd, Color3::blue(), Color4::clear());
				Draw::axes(rd);
			}
		}
	}

	rd->popState();
}

RBX::Gurnel* RBX::Gurnel::get()
{
	return Datamodel::get()->physicsKernel;
}

RBX::Gurnel::Gurnel()
{
	world = dWorldCreate();

	space = dSimpleSpaceCreate(0);
	contacts = dJointGroupCreate(0);

	worldLimitsY = 200;

	dWorldSetGravity(world, 0, -9.81F, 0);
	dWorldSetAutoDisableFlag(world, 1);
	dWorldSetAutoDisableLinearThreshold(world, 0.02f);
	dWorldSetAutoDisableAngularThreshold(world, 0.02f);
	dWorldSetContactSurfaceLayer(world, 0.02f);
	dWorldSetERP(world, 0.5f);
	dWorldSetCFM(world, 0.1f);
}
