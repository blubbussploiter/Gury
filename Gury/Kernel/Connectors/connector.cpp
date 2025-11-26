#include "../Gury/Kernel/jointsservice.h"
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Rendering/renderPrimitives.h"

void RBX::Connector::setParentNode(OffsetBodyNode* parentNode)
{
	this->parentNode = parentNode;
}

CoordinateFrame RBX::Connector::getInterceptPosition()
{
	return (prim0->worldPosition.translation + prim1->worldPosition.translation) / 2;
}

bool RBX::Connector::connected()
{
	if (JointsService::areConnectedPrimitives(prim0, prim1)) return 1;
	if (prim0->body && prim1->body)
	{
		return JointsService::areConnectedBodies(prim0->body, prim1->body);
	}
	return 0;
}

RBX::Connector* RBX::Connector::getConnectingConnector(RBX::Primitive* prim)
{
	if (prim->body)
	{

		void* rawData = prim->getUserdata();
		if (rawData)
		{
			RBX::Instance* instance = static_cast<Instance*>(rawData);

			if (RBX::IsA<Connector>(instance))
				return (Connector*)instance;

		}
	}
	return 0;
}

template <typename T>
void mergeArrays(const Array<T> array0, const Array<T> array1, Array<T>& array2)
{
	for (int i = 0; i < array0.size(); i++)
	{
		T at = array0[i];
		array2.push_back(at);
	}

	for (int i = 0; i < array1.size(); i++)
	{
		T at = array1[i];
		array2.push_back(at);
	}
}

void RBX::Connector::getConnectorTree(Primitive* primitive0, Primitive* primitive1, Array<RBX::Connector*>& connectors)
{
	Instances* children = JointsService::get()->children;
	for (size_t i = 0; i < children->size(); i++)
	{
		Instance* instance = children->at(i);
		if (instance)
		{
			Connector* connector = RBXCast<Connector>(instance);
			if (connector)
			{
				if (connector->prim0 == primitive0 || connector->prim1 == primitive1 ||
					connector->prim0 == primitive1 || connector->prim1 == primitive0)
				{
					if (connectors.contains(connector))
					{
						RBX::StandardOut::print(RBX::MESSAGE_INFO, "Already contains this");
						return;
					}
					else
					{
						connectors.push_back(connector);
						getConnectorTree(connector->prim0, connector->prim1, connectors);
					}
				}
			}
		}
	}
}

RBX::OffsetBodyNode* RBX::Connector::constructParentNode(RBX::Primitive* prim0, RBX::Primitive* prim1)
{
	return 0;
}

void RBX::Connector::build()
{

	if (prim0 && prim1)
	{

		prim0->attachLink(this);
		prim1->attachLink(this);

	}

}