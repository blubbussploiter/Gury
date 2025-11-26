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

	class OffsetBodyNode;
	class PartInstance;

	class Connector : public Instance
	{
	private:
		friend class JointsService;
		friend class SnapConnector;

		Array<Primitive*>* primitives;
		Array<Body*>* bodys;

		OffsetBodyNode* parentNode;

		CoordinateFrame center; // for debug

	public:

		bool isAnchored;

		Primitive* prim0, * prim1;
		NormalId connectedAt;

		virtual void build();
		virtual void unlink() {};

		void setParentNode(OffsetBodyNode* parentNode);
		CoordinateFrame getInterceptPosition(); // for debug 
		static Connector* getConnectingConnector(RBX::Primitive* prim);
		static void getConnectorTree(Primitive* primitive0, Primitive* primitive1, Array<RBX::Connector*>& connectors); /* get any connectors possibly connected to this connector */
		static OffsetBodyNode* constructParentNode(RBX::Primitive* prim0, RBX::Primitive* prim1);

		bool connected();

		Connector(Primitive* prim0, Primitive* prim1, NormalId connectedAt) : prim0(prim0), prim1(prim1), connectedAt(connectedAt)
		{
			parentNode = 0;
		}

		~Connector()
		{
			unlink();
		}
	};

	class OffsetBodyNode /* Used for welds, glue and snaps. anything that uses one body and primitive offsets instead of traditional joints */
	{
	private:
		friend class Connector;
		friend class SnapConnector;
		friend class JointsService; 
		Linkage nodeType;
		Array<Primitive*> primitives;
		Body* thisBody;
		CoordinateFrame cofm;
		bool isStatic; /* anchored or not */
	public:
		void addPrimitive(Primitive* primitive);
		void removePrimitive(Primitive* primitive);
		bool containsPrimitive(Primitive* primitive)
		{
			return primitives.contains(primitive);
		}

		void adjustCofm();

		void createBody();

		void doRender(RenderDevice* renderDevice);
		Extents getExtents();

		OffsetBodyNode()
		{
			nodeType = NotLinked;
			primitives = Array<Primitive*>();
			thisBody = 0;
			isStatic = false;
		}
	};

	class JointsService :
		public Instance,
		public Service<JointsService>
	{
	public:
		Array<Body*> oldBodies; /* bodies from old primitives before connector built */
		Array<OffsetBodyNode*> offsetBodyNodes; /* `nodes` of connectors for building them later on */
	public:

		dJointGroupID joints;
		std::vector<PVInstance*> intersectingGuys;

		BuildTime buildTime;

		class Builder
		{
		public:
			static void recursiveSiblingCheck(bool& endResult, Primitive* primitive, Connector* link, Connector* rootLink=0);
			static bool connectedBySiblings(Connector* link0, Connector* link1, bool recursive);
			static bool checkLinks(Primitive* primitive, Connector* link);
			static bool checkTwoLinks(Primitive* primitive, Connector* link0, Connector* link1);

			static OffsetBodyNode* grabCurrentNode(Primitive* primitive);

			static void shouldBeStatic(OffsetBodyNode* thisNode);

			static bool CollidesWith(Primitive* primitive, OffsetBodyNode* primitiveNode);

			static bool Collides(Primitive* primitive0, Primitive* primitive1);
			static bool Collides(OffsetBodyNode* node0, OffsetBodyNode* node1);

			static void getKernelWorldContacts();

			static void buildInstancesJoints(Instances* instances);
			static void buildGlobalJoints();
		};

		void snap(PartInstance* part1, PartInstance* part2);

		bool inStaticNode(Primitive* primitive);

		void setBuildTime(BuildTime b) {
			buildTime = b;
		}

		BuildTime getBuildTime() {
			return buildTime;
		}

		int getNumConnectors()
		{
			return getChildren()->size(); /* inaccurate lol (2025: actually pretty accurate you nob) */
		}

		Connector* getConnecting(Primitive* primitive);

		bool arePrimitivesConnected(Primitive* primitive0, Primitive* primitive1);

		void addConnector(Connector* connector);
		void addConnectorNode(OffsetBodyNode* newNode);
		void removeConnectorNode(OffsetBodyNode* newNode);

		void linkNodes();
		void linkNode(OffsetBodyNode* connectorNode);

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
			//buildTime = NoBuild;
		}

		RTTR_ENABLE(RBX::Instance)
	};
}