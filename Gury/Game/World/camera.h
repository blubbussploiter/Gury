
#pragma once

/* most of the code from https://github.com/Vulpovile/Blocks3D/blob/0b8847cd8e7d5726870e9f65c558872e0eaf477e/src/source/CameraController.cpp */

#include <G3DAll.h>

#include "ICameraOwner.h"

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Objects/controller.h"
#include "../Gury/Game/Objects/part.h"

#include "sounds.h"

#define CAM_ZOOM_MIN 0.5f
#define CAM_ZOOM_MAX 1000.f /* should be dependant on extents */

namespace RBX
{
	enum CameraType
	{
		Fixed,
		Follow
	};

	class Camera : 
		public RBX::Controller
	{
		RTTR_ENABLE(RBX::Controller)
	private:

		CoordinateFrame startFrame;
		Vector3 startFocus;

		POINT oldMouse;
		POINT mouse;
	public:

		bool panning;

		GCamera* camera;
		CameraType cameraType;

		float oldZoom;
		float yaw, pitch, zoom;

		RBX::PartInstance* cameraSubject;

		Vector3 goal;
		Vector3 translation;

		CoordinateFrame cframe;

		/* blocks3d stuff */

		void doCameraCollisionLogic(); /* player cool camera, cant go out of bounds or nothin */
		bool canZoom(bool inout); /* check if its colliding or will collide */

		void lookAt(const Vector3& position);
		void setFrame(const CoordinateFrame& cf);

		void occlude();

		CoordinateFrame getCoordinateFrame();
		CoordinateFrame getFocus() { return goal; }

		void characterFade();

		void setCoordinateFrame(CoordinateFrame cf)
		{
			if (startFrame.isIdentity())
			{
				startFrame = cf;
			}
			cframe = cf;
			camera->setCoordinateFrame(cframe);
		}

		void setFocus(CoordinateFrame cf)
		{
			if (!startFocus)
			{
				startFocus = cf.translation;
			}
			goal = cf.translation;
			cframe.lookAt(goal);
			camera->setCoordinateFrame(cframe);
		}

		void refreshZoom(const CoordinateFrame& frame);

		void pan(CoordinateFrame* frame, float spdX, float spdY, bool lookAt = true);
		void panLock(CoordinateFrame* frame, float spdX, float spdY);

		void Zoom(short delta);

		/* gury stuff */

		void lookAtSelected();
		void lookAtSelectableArray(ISelectableArray array);

		void tiltRight(double deg = 5);
		void tiltLeft(double deg = 5);

		void tiltUp(double deg = 5);
		void tiltDown(double deg = 5);

		void cam_zoom(bool inout);

		void reset();

		void update(bool rightMouseDown);
		void follow();

		void updateView()
		{ 
			if (!camera)
			{
				camera = new GCamera();
			}
		}
		GCamera* getCamera() { return camera; }

		float getLerp() { return 0.49999998f; }

		virtual void doMove();

		void zoomExtents();
		void zoomExtents(RBX::Extents extents, bool lerp=1);

		void setImageServerViewNoLerp(CoordinateFrame modelCoord);
		RBX::ICameraOwner* getCameraOwner();

		static RBX::Camera* get();
		static RBX::Sound* switch3;

		Camera() : goal(Vector3(0, 0, 0)), yaw(0.f), pitch(0.f), zoom(14.f)
		{
			cameraType = CameraType::Fixed;
			cameraSubject = 0;
			ZeroMemory(&oldMouse, sizeof(POINT));
			ZeroMemory(&mouse, sizeof(POINT));
			setSpeed(2.5f);
			setClassName("Camera");
			setName("Camera");
		}

		virtual ~Camera() {}
	};
}