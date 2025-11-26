#include "imageMasher.h"

static void cImgRead(cimg_library::CImg<float>& cimage, const G3D::GImage& image)
{
	if (cimage._width == image.width && cimage._height == image.height)
	{
		cimg_forXY(cimage, x, y)
		{
			G3D::Color4uint8 color = image.pixel4(x, y);
			cimage(x, y, 0, 0) = color.r;
			cimage(x, y, 0, 1) = color.g;
			cimage(x, y, 0, 2) = color.b;
			cimage(x, y, 0, 3) = color.a;
		}
	}
}

static void cImgWrite(const cimg_library::CImg<float>& cimage, G3D::GImage& image)
{
	if (cimage._width == image.width && cimage._height == image.height)
	{
		cimg_forXY(cimage, x, y)
		{
			image.pixel4()[x + y * image.width].r = cimage(x, y, 0, 0);
			image.pixel4()[x + y * image.width].g = cimage(x, y, 0, 1);
			image.pixel4()[x + y * image.width].b = cimage(x, y, 0, 2);
			image.pixel4()[x + y * image.width].a = cimage(x, y, 0, 3);
		}
	}
}

void RBX::ImageMasher::mash(G3D::GImage foreground, G3D::GImage background, G3D::GImage& output, float opacity)
{
	if (background.height == foreground.height &&
		background.width == foreground.width)
	{
		cimg_library::CImg<float> cImageBackground(background.width, background.height, 1, 4);
		cimg_library::CImg<float> cImageForeground(foreground.width, foreground.height, 1, 4);

		cImgRead(cImageBackground, background);
		cImgRead(cImageForeground, foreground);

		const cimg_library::CImg<> fg_rgb = cImageForeground.get_shared_channels(0, 2);
		const cimg_library::CImg<> fg_a = cImageForeground.get_shared_channel(3);

		cimg_library::CImg<float> result(cImageBackground);

		result.draw_image(0, 0, fg_rgb, fg_a, opacity, 255);

		cImgWrite(result, output);
	}
}
