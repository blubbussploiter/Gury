
#pragma once

#include <map>
#include <G3DAll.h>

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

		class BrickMap
		{
		public:

			std::map<Number, Color3> colors;

			BrickColor brickColorFromNumber(int number);
			BrickColor brickColorFromName(std::string name);
			Color3 fromNumber(int number);
			Color3 fromName(std::string name);

			void insert(std::string name, int number, Color3 color);

			static BrickMap* get();

			BrickMap();
		};

	public:

		Number number;
		Color3 color;

		BrickColor() {}
	};

	static bool operator<(const RBX::BrickColor::Number& x, const RBX::BrickColor::Number& y)
	{
		return x.number < y.number;
	}
}