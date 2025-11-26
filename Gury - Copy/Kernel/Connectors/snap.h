#pragma once

#include "../Gury/Kernel/jointsservice.h"

namespace RBX
{
	class SnapConnector : public Connector
	{
	private:
		bool anchored;

	public:

		static SnapConnector* getConnectingSnap(Primitive* prim);

		void build();
		void link();
		void unlink();

		SnapConnector(Primitive* prim0, Primitive* prim1, NormalId connectedAt) : Connector(prim0, prim1, connectedAt)
		{
			primitives = new Array<Primitive*>();
			bodys = new Array<Body*>();
			setClassName("Snap");
			setName("Snap");
		}

	};
}