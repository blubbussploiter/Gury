
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Rendering/renderScene.h"
#include "../Game/globalSounds.h"

#include "kernel.h"
#include "primitive.h"

#define ENGINE_BOUNDS_Y 200 

void RBX::Gurnel::addPrimitive(Primitive* primitive)
{
	objects.append(primitive);
}

void RBX::Gurnel::removePrimitive(Primitive* primitive)
{
	primitive->destroyPrimitive();
	objects.remove(objects.findIndex(primitive));
}

void RBX::Gurnel::addQueuedPrimitive(Primitive* primitive)
{
	objectQueue.append(primitive);
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

	/* (re) */

	spawnWorld();

	/* step */

	for (int i = 0; i < objects.size(); i++)
	{
		Primitive* prim = objects[i];
		if (!outOfBoundCheck(prim))
		{
			prim->step();
		}
	}
}

void RBX::Gurnel::spawnWorld()
{
	/* not going by very inconsistent primitives anymore, going by pvinstances in scene */

	RBX::Instances instances = Scene::get()->getArrayOfObjects();
	for (unsigned int i = 0; i < instances.size(); i++)
	{
		PVInstance* pv = toInstance<PVInstance>(instances.at(i));
		if (pv)
		{
			pv->initializeForKernel();
		}
	}
}

bool RBX::Gurnel::outOfBoundCheck(Primitive* object)
{
	if (object->pv->position.translation.y <= -ENGINE_BOUNDS_Y)
	{
		if (object->body)
		{
			object->body->destroyBody();
		}

		Instance* instance = (Instance*)object->getUserdata();
		if (instance)
		{
			instance->remove();
			GlobalSounds::pageTurn->play();
		}

		removePrimitive(object);
		return 1;
	}
	return 0;
}

int RBX::Gurnel::getPrimitivesInWorld()
{
	return objects.size();
}

int RBX::Gurnel::getBodiesInWorld()
{
	int hasAttachedBody = 0;
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		Primitive* primitive = objects[i];
		if (primitive && primitive->body)
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

void RBX::Gurnel::collisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	int i;

	Primitive* prim0 = (Primitive*)dGeomGetData(o1);
	Primitive* prim1 = (Primitive*)dGeomGetData(o2);

	if (!prim0 || !prim1) return;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	if (b1 && b2 && dAreConnected(b1, b2))
		return;

	dContact contact[4];
	int numc = dCollide(o1, o2, 4, &contact[0].geom, sizeof(dContact));

	if (numc > 0)
	{
		for (i = 0; i < numc; i++)
		{

			contact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;

			// Define contact surface properties

			float fric = (prim0->friction + prim1->friction) / 2;
			float elas = (prim0->elasticity + prim1->elasticity) / 2;

			contact[i].surface.bounce = elas; //Elasticity
			contact[i].surface.mu = fric; //Friction
			contact[i].surface.slip1 = 0.0f;
			contact[i].surface.slip2 = 0.0f;
			contact[i].surface.soft_erp = 0.8f;
			contact[i].surface.soft_cfm = 0.001f;

			dJointID c = dJointCreateContact(
				Gurnel::get()->world,
				Gurnel::get()->contacts,
				contact + i);

			dJointAttach(c, b1, b2);
		}

	}
}

void RBX::Gurnel::cleanup()
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

	for (unsigned int i = 0; i < objects.size(); i++)
	{
		Primitive* primitive = objects[i];
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
