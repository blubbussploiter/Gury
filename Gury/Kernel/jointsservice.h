#pragma once

#include "../Gury/Game/Services/service.h"
#include "../Gury/Game/Objects/PVInstance/pvinstance.h"

#include "kernel.h"
#include "primitive.h"

namespace RBX
{

	enum Linkage
	{
		NotLinked,
		Snapped,
		Welded,
		Glued,
		Motored,
		Hinged
	};

	enum BuildTime
	{
		Build, /* not real - built eitherway on reset, just for fun! */
		NoBuild /* dont build joints AT ALL */
	};

	class Connector : public Instance
	{
		friend class SnapConnector;
	private:

		CoordinateFrame center; // for debug

	public:

		bool isAnchored;

		Primitive* prim0, * prim1;
		NormalId connectedAt;

		virtual void build() {};
		virtual void unlink() {};

		void onRemove() {
			unlink();
		}

		bool diagPrimitivesAreTouching();

		void diagRender(RenderDevice* rd);
		void diagRenderPrimitiveOutlines(RenderDevice* rd);

		CoordinateFrame getInterceptPosition(); // for debug 

		bool connected();

		Connector(Primitive* prim0, Primitive* prim1, NormalId connectedAt) : prim0(prim0), prim1(prim1), connectedAt(connectedAt)
		{
		}
	};

	class JointsService :
		public Instance,
		public Service<JointsService>
	{
	public:
		Array<Body*> old_Bodies; /* bodies from old primitives before connector built */
	public:

		dJointGroupID joints;
		std::vector<PVInstance*> intersectingGuys;

		BuildTime buildTime;

		class Experiment
		{
		public:
			static bool areIntersecting(PVInstance* pv1, PVInstance* pv2);
			static void getKernelWorldContacts();
			static void buildInstancesJoints(Instances instances);
			static void buildGlobalJoints();
		};

		void setBuildTime(BuildTime b) {
			buildTime = b;
		}

		BuildTime getBuildTime() {
			return buildTime;
		}

		Connector* getConnecting(Primitive* primitive);

		void addConnector(Connector* connector);

		void buildConnectors();

		void buildGlobalJoints();

		/* before build() version, looks through children */

		bool areConnectedUnbuiltPrimitives(Primitive* prim0, Primitive* prim1);

		/* static, after build() version */

		static bool areConnectedPrimitives(Primitive* prim0, Primitive* prim1);

		static bool areConnectedBodies(Body* body0, Body* body1);

		static bool bodyIsConnector(Body* body);

		static Linkage makeLinkage(SurfaceType s0, SurfaceType s1);

		static NormalId fromNormal(PVInstance* object, Vector3 normal);

		static Connector* fromLinkageAndPrimitives(Linkage linkage, Primitive* prim0, Primitive* prim1, NormalId surface);

		static JointsService* get();

		JointsService()
		{
			setName("JointsService");
			setClassName("JointsService");
			joints = dJointGroupCreate(0);
			buildTime = Build;
		}

		RTTR_ENABLE(RBX::Instance)
	};
}