#define cimg_use_png

#include <fstream>
#include <cimg/CImg.h>

#include "../Gury/Game/Objects/PVInstance/pvinstance.h"
#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/strings.h"

#include "textureReserve.h"

using namespace RBX;
using namespace RBX::Render;

DEFINE_SINGLETON(SurfaceFactory);
Table<SurfaceType, int> SurfaceFactory::surfaces = Table<SurfaceType, int>();

static void cImgRead(cimg_library::CImg<float>& cimage, const GImage& image)
{
	if (cimage._width == image.width && cimage._height == image.height)
	{
		cimg_forXY(cimage, x, y)
		{
			Color4uint8 color = image.pixel4(x, y);
			cimage(x, y, 0, 0) = color.r;
			cimage(x, y, 0, 1) = color.g;
			cimage(x, y, 0, 2) = color.b;
			cimage(x, y, 0, 3) = color.a;
		}
	}
}

static void cImgWrite(const cimg_library::CImg<float>& cimage, GImage& image)
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

Vector2 RBX::SurfaceFactory::getFaceBounds(NormalId face, Vector3 brickSize)
{
	return (getFacePositionRelative(face, brickSize) + getFaceSizePx(face, brickSize));
}

Vector2 SurfaceFactory::getFaceSizePx(NormalId face, Vector3 brickSize)
{
	Vector2 pictureSize(64, 128);
	switch (face)
	{
	case Top:
	case Bottom:
	{
		return brickSize.zx() * pictureSize;
	}
	case Right:
	case Left:
	{
		return brickSize.zy() * pictureSize;
	}
	case Front:
	case Back:
	{
		return brickSize.xy() * pictureSize;
	}
	}
	return pictureSize;
}

Vector2 SurfaceFactory::getFacePositionRelative(NormalId face, Vector3 brickSize)
{
	switch (face)
	{
	case Front:
	{
		return Vector2::zero(); /* top left */
	}
	case Back:
	{
		Vector2 px = getFaceSizePx(Front, brickSize);
		return Vector2(px.x, 0);
	}
	case Top:
	{
		Vector2 px = getFaceSizePx(Front, brickSize);
		return Vector2(0, px.y / 2);
	}
	case Bottom:
	{
		Vector2 px = getFaceSizePx(Top, brickSize);
		Vector2 px1 = getFaceSizePx(Front, brickSize);
		return Vector2(px.x, px1.y / 2);
	}
	case Left:
	{
		Vector2 px = getFaceSizePx(Bottom, brickSize);
		Vector2 px1 = getFaceSizePx(Left, brickSize);
		return Vector2(0, px.y / 2 + (px1.y / 2));
	}
	case Right:
	{
		Vector2 px = getFaceSizePx(Top, brickSize);
		Vector2 px1 = getFaceSizePx(Left, brickSize);
		Vector2 px2 = getFaceSizePx(Right, brickSize);
		return Vector2(px1.x, px.y / 2 + (px2.y / 2));
	}
	}
	return Vector2();
}

Vector2 RBX::SurfaceFactory::getTextureDimensions(RBX::Shape shape, Vector3 brickSize)
{
	int width = 0, height = 0;

	switch (shape)
	{
	case Block:
	{
		/* calculate size for width */

		int m0 = getFaceBounds(Back, brickSize).x;
		int m1 = getFaceBounds(Bottom, brickSize).x;

		width = std::max(m0, m1);

		/* calculate size for height */

		height = (getFacePositionRelative(Right, brickSize) + getFaceSizePx(Right, brickSize) / 2).y;

		break;
	}
	}

	return Vector2(width, height);
}

void SurfaceFactory::pasteFaceOnBrickTexture(GImage& bimage, GImage surfaceImage, NormalId face, Vector3 brickSize)
{
	Vector2 positionRel = getFacePositionRelative(face, brickSize);
	GImage::pasteSubImage(bimage, surfaceImage, positionRel.x, positionRel.y, 0, 0, surfaceImage.width, surfaceImage.height);
}

void SurfaceFactory::allocateBrickTexture(RBX::Shape shape, Vector3 brickSize, GImage& bimage)
{
	Vector2 dim = getTextureDimensions(shape, brickSize);
	bimage = GImage(dim.x, dim.y, 4);
}

int SurfaceFactory::generateBrickTexture(PVInstance* pv, RBX::Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom)
{
	GImage brickTexture;

	allocateBrickTexture(shape, brickSize, brickTexture);
	if (brickTexture.width > 0 && brickTexture.height > 0 && brickTexture.channels > 0)
	{
		bricks.push_back(pv);
		pasteSurfaces(brickTexture, shape, brickSize, brickColor, front, back, left, right, top, bottom);
		return Render::TextureReserve::get()->bindTexture(brickTexture);
	}
	return -1;
}

void RBX::SurfaceFactory::editBrickTexture(PVInstance* pv, int textureIndex, RBX::Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom)
{
	auto reserve = Render::TextureReserve::get();

	if (textureIndex != -1)
	{

		GImage brickTexture = reserve->getTexture(textureIndex);
		Vector2 dimensions = getTextureDimensions(shape, brickSize);
		brickTexture.resize(dimensions.x, dimensions.y, 4);

		if (brickTexture.width > 0 && brickTexture.height > 0 && brickTexture.channels > 0)
		{

			pasteSurfaces(brickTexture, shape, brickSize, brickColor, front, back, left, right, top, bottom);
			regen(pv);

			reserve->setTexture(textureIndex, brickTexture);
			dirty = true; /* regen */
		}
	}

}

void RBX::SurfaceFactory::regen(PVInstance* current)
{
	for (int i = 0; i < bricks.size(); i++)
	{
		PVInstance* brick = bricks[i];
		if (brick)
		{
			if (current && brick != current)
			{
				brick->edit();
			}
		}
	}
}

void SurfaceFactory::pasteSurface(NormalId face, SurfaceType surface, Color4 color, Vector3 brickSize, GImage& image)
{
	int fixedWidth, fixedHeight, width, height, channels;

	GImage surfaceTexture, fixedTexture, texture = getSurfaceFromFile(surface);

	width = texture.width;
	height = texture.height;

	Vector2 size = getFaceSizePx(face, brickSize) / Vector2(width, height);

	fixedWidth = int(size.x * width);
	fixedHeight = int(size.y * height) / 2;
	channels = texture.channels;

	surfaceTexture = GImage(fixedWidth, fixedHeight, 4);
	GImage::makeCheckerboard(surfaceTexture, 1, color, color);

	/* write surface to temporary image */

	if (surface != Smooth)
	{
			
		cimg_library::CImg<float> background(fixedWidth, fixedHeight, 1, 4);
		cimg_library::CImg<float> foreground(width, height, 1, 4);

		cImgRead(background, surfaceTexture);
		cImgRead(foreground, texture);

		const cimg_library::CImg<> fg_rgb = foreground.get_shared_channels(0, 2);
		const cimg_library::CImg<> fg_a = foreground.get_shared_channel(3);

		cimg_library::CImg<float> result(background);

		for (int y = 0; y < size.y; y++)
		{
			for (int x = 0; x < size.x; x++)
			{
				result.draw_image(x * width, y * height, fg_rgb, fg_a, 0.891f, 255);
			}
		}

		cImgWrite(result, surfaceTexture);

	}

	pasteFaceOnBrickTexture(image, surfaceTexture, face, brickSize);

}

void RBX::SurfaceFactory::pasteSurfaces(GImage& brickTexture, RBX::Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom)
{
	switch (shape)
	{
	case Block:
	{

		pasteSurface(Front, front, brickColor, brickSize, brickTexture);
		pasteSurface(Back, back, brickColor, brickSize, brickTexture);
		pasteSurface(Left, left, brickColor, brickSize, brickTexture);
		pasteSurface(Right, right, brickColor, brickSize, brickTexture);
		pasteSurface(Top, top, brickColor, brickSize, brickTexture);
		pasteSurface(Bottom, bottom, brickColor, brickSize, brickTexture);

		break;
	}
	}
	if (brickTexture.channels != 4)
	{
		RBX::StandardOut::print(MESSAGE_ERROR, "Failed to generate brick texture!");
	}
}