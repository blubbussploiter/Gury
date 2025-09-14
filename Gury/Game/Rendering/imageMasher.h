#pragma once

#define cimg_use_png

#include <fstream>

#include <G3D/GImage.h>
#include <cimg/CImg.h>

namespace RBX
{
	class ImageMasher
	{
	public:

		static void mash(G3D::GImage foreground, G3D::GImage background, G3D::GImage& output, float opacity);

	};
}