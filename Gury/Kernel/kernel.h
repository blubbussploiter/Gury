#pragma once

/* Gurnel (Gury kernel) written by Billy Brickhoffen
			(2023 - now)
*/

#define toDMatrix3(rotation) { rotation[0][0], rotation[0][1], rotation[0][2], 0,  rotation[1][0], rotation[1][1], rotation[1][2], 0, rotation[2][0], rotation[2][1], rotation[2][2], 0 }

#include "body.h"

namespace RBX
{

	/* my cool physics engine :) */

	class Gurnel 
	{
	private:

		Array<Primitive*> primitives;
		Array<Primitive*> garbagePrimitiveQueue;
		Array<Body*> garbageBodyQueue;

	public:
		friend class Body;
		
		dWorldID world;
		dSpaceID space;
		dJointGroupID contacts;

		float worldLimitsY;

		void close();

		void diag_renderObjects(RenderDevice* rd);

		void addPrimitive(Primitive* primitive);
		void removePrimitive(Primitive* primitive);


		void collectGarbage();
		void haltKernel();

		void step(float stepInS=0.02f, int iterations=64);

		void afterStep();
		void markForReset(bool shouldReset);

		void spawnWorld();

		float getGravity();

		bool outOfBoundCheck(Primitive* object);

		int getPrimitivesInWorld();

		int getBodiesInWorld();

		static void collisionCallback(void* data, dGeomID o1, dGeomID o2);

		static Gurnel* get();

		Gurnel();
	};
}