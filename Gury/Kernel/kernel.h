#pragma once

/* Gurnel (Gury kernel) written by Whirlpool
			(2023 - now)
*/

#define toDMatrix3(rotation) { rotation[0][0], rotation[0][1], rotation[0][2], 0,  rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 }

#include "body.h"

namespace RBX
{

	/* my cool physics engine :) */

	class Gurnel 
	{
	public:

		Array<Primitive*> objects;
		Array<Primitive*> objectQueue;
		
		dWorldID world;
		dSpaceID space;
		dJointGroupID contacts;

		void cleanup();

		void diag_renderObjects(RenderDevice* rd);

		void addPrimitive(Primitive* primitive);

		void removePrimitive(Primitive* primitive);

		void addQueuedPrimitive(Primitive* primitive);

		void step(float stepInS=0.02f, int iterations=64);

		void afterStep();

		void spawnWorld();

		float getGravity();

		bool outOfBoundCheck(Primitive* object);

		int getPrimitivesInWorld();

		int getBodiesInWorld();

		static void collisionCallback(void* data, dGeomID o1, dGeomID o2);

		static Gurnel* get();

		Gurnel()
		{
			world = dWorldCreate();

			space = dHashSpaceCreate(0);
			dHashSpaceSetLevels(space, 1, 4);

			contacts = dJointGroupCreate(0);

			dWorldSetGravity(world, 0, -9.81F, 0);
			dWorldSetAutoDisableFlag(world, 1);
			dWorldSetAutoDisableLinearThreshold(world, 0.02f);
			dWorldSetAutoDisableAngularThreshold(world, 0.02f);
			dWorldSetContactSurfaceLayer(world, 0.01f);
			dWorldSetERP(world, 0.8f);
			dWorldSetCFM(world, 1.0f);
		}
	};
}