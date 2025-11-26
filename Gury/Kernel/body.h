#pragma once

#include <G3DAll.h>
#include "ode/ode.h"

#include "../Gury/Game/Reflection/signal.h"

namespace RBX
{
	class Primitive;

	class Velocity
	{
	public:
		Vector3 linear;
		Vector3 rotational;
		Velocity()
		{
			linear = Vector3::zero();
			rotational = Vector3::zero();
		}
		Velocity(Vector3 linear, Vector3 rotational) : linear(linear), rotational(rotational)
		{
		}
	};

	class PV
	{
	public:
		Velocity velocity;
		CoordinateFrame position;
		PV()
		{
			velocity = Velocity();
			position = CoordinateFrame();
		}
	};

	class Body
	{
	private:
		friend class Gurnel;
		friend class SnapConnector;

	public:

		dBodyID body;
		PV* pv;

		Vector3 size;
		G3D::Array<Primitive*> attachedPrimitives;

		void* ud;
		bool disabled, wantsReset; /* anchored */

		void doDestroy();

	public:

		dMass getMass();

		float getFloatMass();

		bool created() const;

		void* getUserdata();

		bool isValid() const;

		void modifyUserdata(void* data);

		void modifyBodyData(void* data);

		void modifyVelocity(Velocity velocity);

		void modifyPosition(Vector3 position);

		void modifyRotation(Matrix3 rotation);

		void modifyPosition(CoordinateFrame position);

		void modifyPV(PV pv);

		void modifySize(Vector3 size);

		void modifyMass(dMass mass);

		void modifyfMass(float mass);

		void applyTorque(Vector3 torque);

		void applyForce(Vector3 force);

		void applyForceAtPosition(Vector3 force, Vector3 position);

		void applyForceAtRelativePosition(Vector3 force, Vector3 relativePosition);

		bool atRest()
		{
			return (getExactLinearVelocity() == Vector3::zero());
		}

		Vector3 getTorque();

		Vector3 getForce();

		Vector3 getPosition();

		Vector3 getExactLinearVelocity()
		{
			if (body)
			{
				const dReal* v = dBodyGetLinearVel(body);
				return Vector3(v[0], v[1], v[2]);
			}
			return Vector3::zero();
		}

		Vector3 getExactRotationalVelocity()
		{
			if (body)
			{
				const dReal* v = dBodyGetAngularVel(body);
				return Vector3(v[0], v[1], v[2]);
			}
			return Vector3::zero();
		}

		Matrix3 getMoment();

		void createBody(Vector3 size);

		void destroyBody();

		void attachPrimitive(Primitive* primitive);

		void detachPrimitive(Primitive* primitive);

		void setDisabled(bool disabled);

		void step();

		Body()
		{
			pv = new PV();
			body = 0;
		}
	};

}