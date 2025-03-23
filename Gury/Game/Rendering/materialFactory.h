#pragma once

#include "material.h"

namespace RBX
{
	namespace Render
	{
		template <typename Object>
		class MaterialFactory
		{
		private:
			Material* material;
		public:

			Material* getMaterial()
			{
				if (!material)
				{
					material = new Material();
				}
				return material;
			}

			static MaterialFactory* factory();
		};
	}
}