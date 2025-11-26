#include "snap.h"
#include "../Gury/Game/Services/stdout.h"

RBX::SnapConnector* RBX::SnapConnector::getConnectingSnap(Primitive* prim)
{
	return 0;
}

bool isASnapConnector(RBX::Primitive* prim)
{
	return (RBX::SnapConnector::getConnectingSnap(prim) != 0);
}

void RBX::SnapConnector::build()
{
	Connector::build();

	return;

	if (prim0->ud != this && prim1->ud != this)
	{
		OffsetBodyNode* parentNode = constructParentNode(prim0, prim1);

		if (!parentNode)
		{
			//RBX::StandardOut::print(RBX::MESSAGE_ERROR, "No Node");
			//OffsetBodyNode* newNode = new OffsetBodyNode();
			//newNode->nodeType = Snapped;
			//parentNode = newNode;
		}

		//parentNode->addPrimitive(prim0);
		//parentNode->addPrimitive(prim1);

		//setParentNode(parentNode);
		//JointsService::get()->addConnectorNode(parentNode);
	}


}

void RBX::SnapConnector::link()
{
}


void RBX::SnapConnector::unlink()
{
	/* do like JointService::doUnlink(Connector* connector) */
}
