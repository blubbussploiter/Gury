#pragma once

#include "../Gury/Game/World/ICameraOwner.h"
#include "../Gury/Game/Services/stdout.h"

#include "ISelectable.h"

#include "controller.h"
#include "part.h"

namespace RBX
{
	class ModelInstance :
		public RBX::Render::IRenderable,
		public RBX::ISelectable,
		public ICameraOwner
	{
	private:
		RBX::ControllerTypes controllerType;
		RBX::Controller* controller;
	public:

		RBX::PartInstance* primaryPart;

		void breakJoints();
		void buildJoints();

		void createController(); /* recursive */
		void makeController(); /* non recursive */

		bool isLocked();

		void setController(ControllerTypes c) 
		{
			controllerType = c;
		}

		void drawControllerFlag(RenderDevice* rd, Color3 color);
		void render(RenderDevice* rd);

		void setPrimaryPartCFrame(CoordinateFrame cframe);
		CoordinateFrame getPrimaryPartCFrame();

		ControllerTypes getController() { return controllerType; }
		RBX::PartInstance* getPrimaryPartInternal();

		RBX::PartInstance* getPrimaryPart();
		void setPrimaryPart(RBX::PartInstance* part) { primaryPart = part; }

		virtual RBX::ModelInstance* getModel() { return this; }
		virtual RBX::Extents computeCameraOwnerExtents();

		RBX::Extents modelExtents();
		RBX::Extents getModelWorldExtents()
		{
			Extents e = modelExtents();
			return e.toWorldSpace(getPVInstancesCentre(*children));
		}

		void translate(CoordinateFrame cframe);
		
		static void updatePrimaryPart(Instance* _this, Instance* child);
		static void translateInstances(Instances i, PVInstance* rootPart, CoordinateFrame cframe);
		static CoordinateFrame getPVInstancesCentre(Instances pvInstances);
		static Extents getInstancesExtents(Instances i);
		static PartInstance* getRootPart(Instances i);

		Render::Geometry getBoundingBox()
		{
			Extents extents = getInstancesExtents(*children);
			if (extents.low == Vector3::zero() && extents.high == Vector3::zero())
			{
				return Render::Geometry();
			}
			else {
				CoordinateFrame centrePosition = getPVInstancesCentre(*children);
				return Render::Geometry(centrePosition, extents.size(), Block);
			}
		}

		ModelInstance()
		{
			controllerType = ControllerTypes::None;
			controller = nullptr;
			primaryPart = nullptr;
			onChildAdded.connect(updatePrimaryPart);
			setClassName("Model");
			setName("Model");
		}

		virtual ~ModelInstance() { breakJoints(); }

		RBX_CLONE_DEF(ModelInstance)
		RTTR_ENABLE(Instance)
	};

}
