
#include "brickcolor.h"
#include "surfaceFactory.h"
#include "textureReserve.h"
#include "imageMasher.h"

void RBX::BrickColor::BrickAtlasMap::orderInAtlas(Color4 brickColor, SurfaceType surface)
{
	if (!isInAtlas(brickColor, surface))
	{
		GImage orderedTexture = GImage(0, 0, 4);

		orderedTexture.resize(64 + 10, 128 + 1, 4); /* plus 5, 5 */

		Color4uint8 color;
		color.r = brickColor.r * 255;
		color.g = brickColor.g * 255;
		color.b = brickColor.b * 255;
		color.a = brickColor.a * 255;

		GImage::makeCheckerboard(orderedTexture, 1, color, color);
		if (surface != Smooth)
		{
			GImage surfaceTexture = getSurfaceFromFile(surface);
			GImage surfaceCanvas(orderedTexture.width, orderedTexture.height, 4);

			GImage::pasteSubImage(surfaceCanvas, surfaceTexture, 4, 0, 0, 0, surfaceTexture.width, surfaceTexture.height);

			ImageMasher::mash(surfaceCanvas, orderedTexture, orderedTexture, 0.65f);
		}

		int position = Render::TextureReserve::get()->bindTexture(orderedTexture);

		AtlasInfo info;
		info.brickColor = brickColor;
		info.surface = surface;
		info.references++;

		atlasColors.set(position, info);
	}
	else
	{
		uint32_t infoIndex = getInAtlas(brickColor, surface);
		AtlasInfo info;

		info = atlasColors.get(infoIndex);
		info.references++;

		atlasColors.set(infoIndex, info); /* update for new references */
	}
}

bool RBX::BrickColor::BrickAtlasMap::isInAtlas(Color4 brickColor, SurfaceType surface) const
{
	Array<int> keys = atlasColors.getKeys();

	for (int i = 0; i < keys.size(); i++)
	{
		int idx = keys[i];
		AtlasInfo info = atlasColors.get(idx);
		if (info.brickColor == brickColor
			&& info.surface == surface)
		{
			return true;
		}
	}

	return false;
}

uint32_t RBX::BrickColor::BrickAtlasMap::getInAtlas(Color4 color, SurfaceType surface) const
{
	Array<int> keys = atlasColors.getKeys();

	for (int i = 0; i < keys.size(); i++)
	{
		int idx = keys[i];
		AtlasInfo info = atlasColors.get(idx);

		if (info.brickColor == color
			&& info.surface == surface)
		{
			return idx;
		}
	}

	return -1;
}


void RBX::BrickColor::BrickAtlasMap::tryRemove(Color4 brickColor, SurfaceType surface)
{
	uint32_t infoIndex = getInAtlas(brickColor, surface);

	if (infoIndex != -1 && atlasColors.containsKey(infoIndex))
	{
		AtlasInfo info = atlasColors.get(infoIndex);

		info.references--;

		if (info.references <= 0)
		{
			Render::TextureReserve::get()->unbindTexture(infoIndex);
			atlasColors.remove(infoIndex);
		}
		else
		{
			atlasColors.set(infoIndex, info); /* update */
		}
	}
}

void RBX::BrickColor::BrickMap::orderAllInSurface(std::string name,SurfaceType surface)
{
	BrickAtlasMap* map = BrickAtlasMap::get();
	map->orderInAtlas(fromName(name), surface);
}

void RBX::BrickColor::BrickMap::orderAllColors()
{
	//orderAllInSurface("")
}

RBX::BrickColor::BrickAtlasMap* RBX::BrickColor::BrickAtlasMap::get()
{
	return Render::TextureReserve::get()->getColorAtlas();
}
