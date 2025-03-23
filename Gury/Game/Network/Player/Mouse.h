#pragma once

#include <vector>
#include <G3DAll.h>

#include "../Gury/Application/appmanager.h"

#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/Objects/instance.h"
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/World/ray.h"

namespace RBX
{
	class Mouse
	{
	private:

		int currentglId;

		RBX::PVInstance* target;
		Vector3 hitWorld, dir;

		HCURSOR cursor;

	public:

		bool hoveringUI;
		float cx, cy, x, y;

		RBX::PVInstance* getTarget();
		RBX::ModelInstance* getModelTarget();

		Vector3 getDir() { getTarget(); return dir; }
		Vector3 getHit() { getTarget(); return hitWorld; }

		bool inGuryWindow();

		void update(UserInput* ui);
		void updateCursorInfo();
		void render(RenderDevice* rd);

		HCURSOR get_cursor();

		static Mouse* get();

		template<typename IgnoredItem>
		RBX::PVInstance* getTargetWithIgnoreList(std::vector<IgnoredItem*>& ignore)
		{
			Ray ray;
			Rect2D viewport;

			viewport = RBXManager::get()->getApplication()->getViewport();
			Camera* camera = Camera::get();

			ray = camera->camera->worldRay(x, y, viewport);
			target = (PVInstance*)World::getPartFromG3DRay<IgnoredItem>(ray, hitWorld, 0, ignore);

			return target;
		}

		Mouse()
		{

		}
	};

}