
#pragma once

#include <map>
#include <G3DAll.h>

#include "../Objects/PVInstance/pvenums.h"

namespace RBX
{
	class BrickColor
	{
	public:
		class Number
		{
		public:
			std::string name;
			int number;
		};

		class AtlasInfo
		{
		public:

			Color4 brickColor;
			SurfaceType surface;

			int references;

			AtlasInfo()
			{
				surface = SurfaceType::UNDEFINED_SURFACE;
				references = 0;
			}
		};

		class BrickAtlasMap
		{
		public:

			Table<int, AtlasInfo> atlasColors;

			void orderInAtlas(Color4 brickColor, SurfaceType surface);

			bool isInAtlas(Color4 brickColor, SurfaceType surface)  const;
			uint32_t getInAtlas(Color4 brickColor, SurfaceType surface) const;

			void tryRemove(Color4 brickColor, SurfaceType surface);

			static BrickAtlasMap* get();
		};

		class BrickMap
		{
		public:

			std::map<Number, Color3> colors;

			BrickColor brickColorFromNumber(int number);
			BrickColor brickColorFromName(std::string name);
			Color3 fromNumber(int number);
			Color3 fromName(std::string name);

			void insert(std::string name, int number, Color3 color);
			void orderAllInSurface(std::string name, SurfaceType surface);
			void orderAllColors();

			static BrickMap* get();

			BrickMap();
		};

	public:

		Number number;
		Color3 color;

		static BrickMap* getBrickMap()
		{
			return BrickMap::get();
		}
		static BrickAtlasMap* getColorMap()
		{
			return BrickAtlasMap::get();
		}

		BrickColor() {}
	};

	static bool operator<(const RBX::BrickColor::Number& x, const RBX::BrickColor::Number& y)
	{
		return x.number < y.number;
	}


}