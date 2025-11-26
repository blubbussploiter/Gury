#pragma once

#include <g3dall.h>

namespace RBX
{

	class cursify
	{

	public:

		static HBITMAP pngToBitmap(std::string pngPath);

		static HCURSOR createCursor(std::string cursorPathPng);

	};

}