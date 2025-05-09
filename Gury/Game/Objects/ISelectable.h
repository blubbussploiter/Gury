#pragma once

#include <G3DAll.h>

#include "geometry.h"

namespace RBX
{
	class ISelectable
	{
	public:
		bool isSelectable;

		ISelectable()
		{
			isSelectable = true;
		}
	public:

		virtual Render::Geometry getBoundingBox() { return Render::Geometry(); };
		virtual CoordinateFrame getCenter() { return getBoundingBox().cframe; }
		virtual void setCenter(CoordinateFrame) { }

		static Vector3 getCenterOfSelectableObjects(std::vector<ISelectable*> objects)
		{
			Vector3 min, max;
			for (unsigned int i = 0; i < objects.size(); i++) /* modded version of this: https://devforum.roblox.com/t/how-do-i-get-the-middle-of-multiple-parts/2007200/20 */
			{
				ISelectable* selectable = objects.at(i);
				Vector3 pos = selectable->getCenter().translation;

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
			return ((max - min) / 2) + min;
		}
	};

	typedef std::vector<RBX::ISelectable*> ISelectableArray;

}