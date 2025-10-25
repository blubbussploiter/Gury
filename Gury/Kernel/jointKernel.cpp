#include "../Gury/Game/Services/service.h"
#include "../Gury/Game/Rendering/renderScene.h"

#include "jointsservice.h"

void RBX::JointsService::Experiment::getKernelWorldContacts()
{
    Instances instances = WorldScene::get()->getArrayOfObjects();
	buildInstancesJoints(instances);
}

void RBX::JointsService::Experiment::buildInstancesJoints(Instances instances)
{
	for (RBX::Instance* instance1 : instances)
	{
		for (RBX::Instance* instance2 : instances)
		{
			RBX::PVInstance* pvInstance1 = toInstance<PVInstance>(instance1);
			RBX::PVInstance* pvInstance2 = toInstance<PVInstance>(instance2);

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
					Connector* connector = fromLinkageAndPrimitives(link, pvInstance1->primitive, pvInstance2->primitive, n0);
					connector->build();
					JointsService::get()->addConnector(connector);
				}
			}

		}
	}
}

void RBX::JointsService::Experiment::buildGlobalJoints()
{
	/* create joints from contacts */
	getKernelWorldContacts();
}
