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

	class ConnectorNode;

	class Connector : public Instance
	{
		friend class JointsService;
		friend class SnapConnector;
	private:

		Array<Primitive*>* primitives;
		Array<Body*>* bodys;

		ConnectorNode* parentNode;

		CoordinateFrame center; // for debug

	public:

		bool isAnchored;

		Primitive* prim0, * prim1;
		NormalId connectedAt;

		virtual void build();
		virtual void link() {};
		virtual void unlink() {};

		bool diagPrimitivesAreTouching();

		void diagRender(RenderDevice* rd);
		void diagRenderPrimitiveOutlines(RenderDevice* rd);

		CoordinateFrame getInterceptPosition(); // for debug 
		static Connector* getConnectingConnector(RBX::Primitive* prim);
		static ConnectorNode* getParentNode(RBX::Primitive* prim0, RBX::Primitive* prim1);
		static void getConnectingConnectorsByPrim(RBX::Primitive* prim, Array<Connector*>& finalArray);
		static void getConnectingConnectorsByConnector(RBX::Connector* connector, Array<Connector*>& finalArray);

		bool connected();

		Connector(Primitive* prim0, Primitive* prim1, NormalId connectedAt) : prim0(prim0), prim1(prim1), connectedAt(connectedAt)
		{
		}

		~Connector()
		{
			unlink();
		}
	};

	class ConnectorNode
	{
	private:
		Array<Connector*> connectors;
	public:
		void addConnector(Connector* connector)
		{
			connectors.append(connector);
		}
		void removeConnector(Connector* connector)
		{
			if (connectors.contains(connector))
			{
				connectors.fastRemove(connectors.findIndex(connector));
			}
		}
	};

	class JointsService :
		public Instance,
		public Service<JointsService>
	{
	public:
		Array<Body*> old_Bodies; /* bodies from old primitives before connector built */
		Array<ConnectorNode*> connectorNodes; /* `nodes` of connectors for building them later on */
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

		int getNumConnectors()
		{
			return getChildren()->size(); /* inaccurate lol */
		}

		Connector* getConnecting(Primitive* primitive);

		void addConnector(Connector* connector);

		void buildConnectors();

		void buildGlobalJoints();

		void doLink(Connector* connector);

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
			buildTime = NoBuild;
		}

		RTTR_ENABLE(RBX::Instance)
	};
}