#include "model.h"

#include "../Gury/Kernel/jointsservice.h"

#include "../Gury/Game/World/workspace.h"
#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/Controllers/rightkeyboard.h"

#include "keyboard.h"

RTTR_REGISTRATION
{
	rttr::registration::class_ <RBX::ModelInstance>("Model")
		 .constructor<>()
		 .property("primaryPart", &RBX::ModelInstance::getPrimaryPart, &RBX::ModelInstance::setPrimaryPart)(rttr::metadata("Type", RBX::Data))
		 .property("ControllerType", &RBX::ModelInstance::getController, &RBX::ModelInstance::setController)(rttr::metadata("Type", RBX::Behavior))
		 .method("breakJoints", &RBX::ModelInstance::breakJoints);
	rttr::registration::enumeration<RBX::ControllerTypes>("ControllerType")
		(
			rttr::value("None", RBX::None),
			rttr::value("KeyboardRight", RBX::KeyboardRight),
			rttr::value("KeyboardLeft", RBX::KeyboardLeft)
		);
}

void RBX::ModelInstance::createController()
{
	RBX::Instances* children;
	children = getChildren();
	for (unsigned int i = 0; i < children->size(); i++)
	{
		RBX::Instance* child;
		child = children->at(i);
		if (IsA<ModelInstance>(child))
		{
			toInstance<ModelInstance>(child)->makeController();
		}
	}
	makeController();
}

void RBX::ModelInstance::makeController()
{
	if (controller) return;
	switch (controllerType)
	{
	case KeyboardRight:
	{
		RightKeyboardController* rkc = new RightKeyboardController();
		rkc->bindCharacter(this);
		controller = rkc;
		break;
	}
	case KeyboardLeft:
	{
		KeyboardController* lkc = new KeyboardController();
		lkc->bindCharacter(this);
		controller = lkc;
		break;
	}
	default: return;
	}
	//RBX::ControllerService::get()->addController(controller);
}

bool RBX::ModelInstance::isLocked()
{
	size_t locked = 0;
	Instances* pvInstances = new Instances();

	Workspace::getPVInstances(children, pvInstances);

	for (size_t i = 0; i < pvInstances->size(); i++) {
		PVInstance* child = toInstance<PVInstance>(pvInstances->at(i));
		if (child && child->locked)
		{
			locked++;
		}
	}

	return (locked >= pvInstances->size());
}

void RBX::ModelInstance::drawControllerFlag(RenderDevice* rd, Color3 color)
{
	RBX::PVInstance* primaryPart = getPrimaryPart();
	if (!primaryPart) return;

	rd->setObjectToWorldMatrix(primaryPart->getCFrame());
	/* draw the flag stand */

	Draw::line(Line::fromTwoPoints(Vector3::zero(), Vector3(0, 4, 0)), rd, color);
}

void RBX::ModelInstance::render(RenderDevice* rd)
{
	Color3 color;
	switch (controllerType)
	{
	case KeyboardRight: color = Color3::BLUE; break;
	case KeyboardLeft: color = Color3::RED; break;
	case None: return;
	}
	drawControllerFlag(rd, color);
}

void RBX::ModelInstance::setPrimaryPartCFrame(CoordinateFrame cframe)
{
	if (getPrimaryPart())
	{
		primaryPart->setCFrame(cframe);
	}
}

CoordinateFrame RBX::ModelInstance::getPrimaryPartCFrame()
{
	if (getPrimaryPart())
	{
		return primaryPart->getCFrame();
	}
	return CoordinateFrame();
}

RBX::PartInstance* RBX::ModelInstance::getPrimaryPart()
{
	if (!primaryPart)
	{
		primaryPart = getPrimaryPartInternal();
	}
	return primaryPart;
}

RBX::Extents RBX::ModelInstance::computeCameraOwnerExtents()
{
	RBX::PartInstance* primaryPart;
	primaryPart = getPrimaryPart();

	if (primaryPart)
	{
		return primaryPart->getWorldExtents();
	}

	return RBX::Extents();
}

RBX::Extents RBX::ModelInstance::modelExtents()
{
	return getInstancesExtents(*getChildren());
}

void RBX::ModelInstance::updatePrimaryPart(Instance* child)
{
	if (child)
	{
		ModelInstance* parent = toInstance<ModelInstance>(child->parent);
		parent->getPrimaryPartInternal();
	}
}

void RBX::ModelInstance::translateInstances(Instances instances, PVInstance* rootPart, CoordinateFrame cframe)
{
	CoordinateFrame frame = rootPart->getCFrame();
	Instances* children = new Instances();

	Workspace::getPVInstances(&instances, children);

	for (unsigned int i = 0; i < children->size(); i++)
	{
		PVInstance* pv = toInstance<PVInstance>(children->at(i));
		if (pv)
		{
			CoordinateFrame pvFrame = pv->getCFrame();
			Vector3 t = pvFrame.translation - frame.translation;
			Vector3 l = cframe.translation + t;
			Matrix3 r = pvFrame.rotation * cframe.rotation;
			pv->setCFrame(CoordinateFrame(r, l));
		}
	}
}

/* modded version of this: https://devforum.roblox.com/t/how-do-i-get-the-middle-of-multiple-parts/2007200/20 */
CoordinateFrame RBX::ModelInstance::getPVInstancesCentre(Instances instances)
{
	Vector3 min, max;
	Instances* pvInstances = new Instances();

	Workspace::getPVInstances(&instances, pvInstances);

	for (unsigned int i = 0; i < pvInstances->size(); i++)
	{
		PVInstance* pvInstance = toInstance<PVInstance>(pvInstances->at(i));
		if (pvInstance)
		{
			Vector3 pos = pvInstance->getCoordinateFrame().translation;

			if (min == Vector3::zero())
			{
				min = pos;
			}
			else
			{
				if (pos.x < min.x) min.x = pos.x;
				if (pos.y < min.y) min.y = pos.y;
				if (pos.z < min.z) min.z = pos.z;
			}

			if (max == Vector3::zero())
			{
				max = pos;
			}
			else
			{
				if (pos.x > max.x) max.x = pos.x;
				if (pos.y > max.y) max.y = pos.y;
				if (pos.z > max.z) max.z = pos.z;
			}
		}
	}
	return ((max - min) / 2) + min;
}

RBX::Extents RBX::ModelInstance::getInstancesExtents(Instances instances) 
{
	Vector3 min, max;
	Instances* pvs = new Instances();

	Workspace::getPVInstances(&instances, pvs);

	for (unsigned int i = 0; i < pvs->size(); i++)
	{
		PVInstance* pvInstance = toInstance<PVInstance>(pvs->at(i));
		if (pvInstance)
		{
			Extents extents;
			extents = pvInstance->getWorldExtents();

			if (min == Vector3::zero())
			{
				min = extents.low;
			}
			else
			{
				if (extents.low.x < min.x) min.x = extents.low.x;
				if (extents.low.y < min.y) min.y = extents.low.y;
				if (extents.low.z < min.z) min.z = extents.low.z;
			}

			if (max == Vector3::zero())
			{
				max = extents.high;
			}
			else
			{
				if (extents.high.x > max.x) max.x = extents.high.x;
				if (extents.high.y > max.y) max.y = extents.high.y;
				if (extents.high.z > max.z) max.z = extents.high.z;
			}
		}
	}
	if (min == Vector3::zero() && max == Vector3::zero()){
		return Extents();
	}
	else{
		return Extents(min / 2, max / 2);
	}
}

RBX::PVInstance* RBX::ModelInstance::getRootPart(Instances i)
{
	PVInstance* result = 0;
	Instances* children = new Instances();

	float lastArea = -1;
	Workspace::getPVInstances(&i, children);

	for (unsigned int i = 0; i < children->size(); i++)
	{
		PVInstance* pv = (PVInstance*)(children->at(i));
		Extents extents = pv->getWorldExtents();
		float area = extents.area();
		if (area > lastArea)
		{
			lastArea = area;
			result = pv;
		}
	}
	children->clear();
	delete children;
	return result;
}

RBX::PartInstance* RBX::ModelInstance::getPrimaryPartInternal()
{
	PVInstance* rootPart = getRootPart(*getChildren());
	if (!rootPart)
	{
		for (size_t i = 0; i < children->size(); i++)
		{
			Instance* _i = children->at(i);
			if (IsA<PVInstance>(_i))
			{
				rootPart = toInstance<PVInstance>(_i);
			}
		}
	}
	return toInstance<PartInstance>(rootPart);
}

void RBX::ModelInstance::translate(CoordinateFrame cframe)
{
	translateInstances(*children, getPrimaryPart(), cframe);
}

/* joints */

void RBX::ModelInstance::buildJoints()
{
	JointsService::Experiment::buildInstancesJoints(*children);
}

void RBX::ModelInstance::breakJoints()
{
	Instances* instances = new Instances();
	Workspace::getPVInstances(getChildren(), instances);

	for (unsigned int i = 0; i < instances->size(); i++)
	{
		PVInstance* pv = toInstance<PVInstance>(instances->at(i));
		Body* body = pv->primitive->body;

		if (body)
		{
			void* ud = body->getUserdata();
			Connector* connector = (Connector*)(ud);
			if(connector) connector->unlink();
		}
	}
}