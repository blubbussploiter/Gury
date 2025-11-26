#include "../Gury/Game/Services/service.h"
#include "../Gury/Game/Rendering/renderScene.h"

#include "jointsservice.h"

void RBX::JointsService::Builder::recursiveSiblingCheck(bool& endResult, Primitive* primitive, Connector* link, Connector* rootLink)
{
	if (primitive)
	{
		for (int i = 0; i < primitive->links.size(); i++)
		{
			Connector* otherLink = primitive->links[i];

			if (otherLink == link || otherLink == rootLink)
			{
				return;
			}
			
			if (otherLink)
			{

				if (checkLinks(otherLink->prim0, link))
				{
					endResult = true;
				}
				else if (checkLinks(otherLink->prim1, link))
				{
					endResult = true;
				}
				else
				{
					recursiveSiblingCheck(endResult, otherLink->prim0, link, otherLink);
					recursiveSiblingCheck(endResult, otherLink->prim1, link, otherLink);
				}
			}
		}
	}
}

bool RBX::JointsService::Builder::connectedBySiblings(Connector* link0, Connector* link1, bool recursive)
{
	if (!recursive)
	{
		if (link0)
		{
			if (checkTwoLinks(link0->prim0, link0, link1))
			{
				return true;
			}
			if (checkTwoLinks(link0->prim1, link0, link1))
			{
				return true;
			}
		}

		if (link1)
		{
			if (checkTwoLinks(link1->prim0, link0, link1))
			{
				return true;
			}
			if (checkTwoLinks(link1->prim1, link0, link1))
			{
				return true;
			}
		}
	}
	else {
		bool areConnected = false;
		recursiveSiblingCheck(areConnected, link0->prim0, link1);
		recursiveSiblingCheck(areConnected, link0->prim1, link1);
		return areConnected;
	}

	return false;
}

bool RBX::JointsService::Builder::checkLinks(Primitive* primitive, Connector* link)
{
	return primitive->links.contains(link);
}

bool RBX::JointsService::Builder::checkTwoLinks(Primitive* primitive, Connector* link0, Connector* link1)
{
	return checkLinks(primitive, link0) && checkLinks(primitive,link1);
}

RBX::OffsetBodyNode* RBX::JointsService::Builder::grabCurrentNode(Primitive* primitive)
{
	Array<OffsetBodyNode*> nodes = JointsService::get()->offsetBodyNodes;
	for (int i = 0; i < nodes.size(); i++)
	{
		OffsetBodyNode* node = nodes[i];
		if (node)
		{
			if (CollidesWith(primitive, node))
			{
				return node;
			}
		}
	}
	return 0;
}

void RBX::JointsService::Builder::shouldBeStatic(OffsetBodyNode* thisNode)
{
	Array<OffsetBodyNode*> nodes = JointsService::get()->offsetBodyNodes;
	for (int i = 0; i < nodes.size(); i++)
	{
		OffsetBodyNode* node = nodes[i];
		if (node)
		{
			if (Collides(thisNode, node))
			{
				if (node->isStatic)
				{
					thisNode->isStatic = true;
				}
			}
		}
	}
}

bool RBX::JointsService::Builder::CollidesWith(Primitive* primitive, OffsetBodyNode* primitiveNode)
{
	if (primitiveNode)
	{
		for (int i = 0; i < primitiveNode->primitives.size(); i++)
		{
			Primitive* p = primitiveNode->primitives[i];
			if (Collides(p, primitive))
			{
				return true;
			}
		}
	}
	return false;
}

bool RBX::JointsService::Builder::Collides(Primitive* primitive0, Primitive* primitive1)
{
	CoordinateFrame cframe0, cframe1;

	cframe0 = primitive0->getPosition();
	cframe1 = primitive1->getPosition();

	Vector3 modifiedSize1 = primitive0->size * 1.05f;
	Vector3 modifiedSize2 = primitive1->size * 1.05f;
	Box modifiedBox1 = cframe0.toWorldSpace(Box(-modifiedSize1, modifiedSize1));
	Box modifiedBox2 = cframe1.toWorldSpace(Box(-modifiedSize2, modifiedSize2));

	Array<Vector3> contactPoints, contactNormals;

	/* get two contact normals (for linkage) */

	CollisionDetection::penetrationDepthForFixedBoxFixedBox(modifiedBox1, modifiedBox2, contactPoints, contactNormals);

	return contactNormals.size() != 0;
}

bool RBX::JointsService::Builder::Collides(OffsetBodyNode* node0, OffsetBodyNode* node1)
{
	if (node0 && node1)
	{
		for (int i = 0; i < node0->primitives.size(); i++)
		{
			Primitive* p = node0->primitives[i];
			if (CollidesWith(p, node1))
			{
				return true;
			}
		}
	}
	return false;
}

void RBX::JointsService::Builder::getKernelWorldContacts()
{
    Instances* instances = WorldScene::get()->getArrayOfObjects();
	buildInstancesJoints(instances);
}

void RBX::JointsService::Builder::buildInstancesJoints(Instances* instances)
{
	OffsetBodyNode* currentSnapNode = nullptr;

	for (RBX::Instance* instance1 : *instances)
	{
		for (RBX::Instance* instance2 : *instances)
		{
			RBX::PartInstance* pvInstance1 = toInstance<PartInstance>(instance1);
			RBX::PartInstance* pvInstance2 = toInstance<PartInstance>(instance2);

			if (pvInstance1 == 0 || pvInstance2 == 0) continue;
			if (pvInstance1 == pvInstance2) continue;

			if (pvInstance1->anchored && pvInstance2->anchored)
			{
				continue;
			}

			CoordinateFrame pvInstance1CFrame, pvInstance2CFrame;
			pvInstance1CFrame = pvInstance1->getCFrame();
			pvInstance2CFrame = pvInstance2->getCFrame();

			/* hacky lil' thing, might not work in the future */

			Vector3 modifiedSize1 = pvInstance1->getSize() * 1.05f;
			Vector3 modifiedSize2 = pvInstance2->getSize() * 1.05f;
			Box modifiedBox1 = pvInstance1CFrame.toWorldSpace(Box(-modifiedSize1, modifiedSize1));
			Box modifiedBox2 = pvInstance2CFrame.toWorldSpace(Box(-modifiedSize2, modifiedSize2));

			Array<Vector3> contactPoints, contactNormals;

			/* get two contact normals (for linkage) */

			CollisionDetection::penetrationDepthForFixedBoxFixedBox(modifiedBox1, modifiedBox2, contactPoints, contactNormals);

			for (int i = 0; i < contactNormals.size(); i++)
			{
				Vector3 normal = contactNormals[i];

				NormalId n0, n1;

				n0 = fromNormal(pvInstance1, normal);
				n1 = fromNormal(pvInstance2, -normal);

				SurfaceType s0, s1;
				s0 = pvInstance1->getSurface(n0);
				s1 = pvInstance2->getSurface(n1);

				Linkage link = makeLinkage(s0, s1);

				if (link != NotLinked)
				{
					if (!JointsService::get()->areConnectedUnbuiltPrimitives(pvInstance1->primitive, pvInstance2->primitive))
					{
						Connector* connector = fromLinkageAndPrimitives(link, pvInstance1->primitive, pvInstance2->primitive, n0);
						connector->build();

						switch (link)
						{
							case Welded:
							case Glued:
							case Snapped:
							{

								OffsetBodyNode* node0 = grabCurrentNode(pvInstance1->primitive);
								OffsetBodyNode* node1 = grabCurrentNode(pvInstance2->primitive);

								if (!node0 && !node1)
								{
									currentSnapNode = new OffsetBodyNode();
								}
								else if (node0 == node1)
								{
									currentSnapNode = node0; 
								}

								if (currentSnapNode)
								{
									shouldBeStatic(currentSnapNode);

									if (pvInstance1->anchored || pvInstance2->anchored)
									{
										currentSnapNode->isStatic = true;
									}

									currentSnapNode->addPrimitive(pvInstance1->primitive);
									currentSnapNode->addPrimitive(pvInstance2->primitive);
									JointsService::get()->addConnectorNode(currentSnapNode);
								}
								break;
							}
						}

						JointsService::get()->addConnector(connector);

					}
				}
			}

		}
	}
}

void RBX::JointsService::Builder::buildGlobalJoints()
{
	/* create joints from contacts */
	getKernelWorldContacts();
	RBX::StandardOut::print(RBX::MESSAGE_INFO, "Builder: Finalized global joint linkage, %d joints, %d nodes", JointsService::get()->getNumConnectors(), JointsService::get()->offsetBodyNodes.size());
}
