#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Reflection/signal.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Lua/singletonType.h"

#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Rendering/brickcolor.h"

namespace RBX
{
	namespace Render
	{

		class TextureReserve
		{
		public:

			class TexturePositionalInformation
			{
			public:
				float x, y, cx, cy;
				TexturePositionalInformation operator+(TexturePositionalInformation& other)
				{
					this->x += other.x;
					this->y += other.y;
					this->cx += other.cx;
					this->cy += other.cy;
					return *this;
				}
				TexturePositionalInformation(Vector2 xy, Vector2 wh)
				{
					x = xy.x;
					y = xy.y;
					cx = wh.x;
					cy = wh.y;
				}
				TexturePositionalInformation(float x, float y, float cx, float cy)
				{
					this->x = x;
					this->y = y;
					this->cx = cx;
					this->cy = cy;
				}
				TexturePositionalInformation()
				{
					x = 0;
					y = 0;
					cx = 0;
					cy = 0;
				}
			};

		private:

			bool dirty;
			Vector2 dimensions;
			Table<int, GImage*> bindedTextures;
			Table<int, TexturePositionalInformation> superTexturePositions;
			TextureRef guryWorldTexture; GImage superTextureData;

			BrickColor::BrickAtlasMap* colorAtlasMap;

		public:

			Table<int, GImage*> getBindedTextures()
			{
				return bindedTextures;
			}

			int getNumTextures()
			{
				return bindedTextures.size();
			}

			void regenWorld();

			/* Remove all attached texCoords from this textureUnit */

			void unbindTexture(int texture)
			{
				if (bindedTextures.containsKey(texture))
				{
					bindedTextures.remove(texture);
					dirty = 1;
				}
			}

			int bindTexture(GImage texture)
			{
				int idx = bindedTextures.size();

				bindedTextures.set(idx, new GImage(texture));
				dirty = 1;

				return idx;
			}

			GImage getTexture(int index)
			{
				if (bindedTextures.containsKey(index))
				{
					GImage* t = bindedTextures.get(index);
					if (t)
					{
						return *t;
					}
				}
				return GImage();
			}

			void setTexture(int index, GImage image)
			{
				if (bindedTextures.size() >= index)
				{
					/* resize */
					bindedTextures.set(index, new GImage(image));
				}
			}

			Vector2 getSuperImageDimensions()
			{
				return Vector2(superTextureData.width, superTextureData.height);
			}

			/* Texture Position Things */

			TexturePositionalInformation getTexturePosition(int index)
			{
				if (superTexturePositions.containsKey(index))
				{
					return superTexturePositions[index];
				}
				return TexturePositionalInformation(0, 0, 0, 0);
			}

			TextureRef getSuperTexture()
			{
				return guryWorldTexture;
			}

			GImage getSuperTextureData()
			{
				return superTextureData;
			}

			TexturePositionalInformation getSurfaceUV(Color4 brickColor, SurfaceType surface, NormalId normalId, Vector2 sizeTile);

			void getSurfaceXXYY(Color4 brickColor, SurfaceType surface, NormalId normalId, Vector2 sizeTile, Vector2& u, Vector2& v);

			Vector2 calculateSuperTextureDimensions();

			void generateSuperTexture();

			BrickColor::BrickAtlasMap* getColorAtlas()
			{
				return colorAtlasMap;
			}

			static TextureReserve* get();

			TextureReserve()
			{
				dirty = false;
				dimensions = Vector2::zero();
				colorAtlasMap = new BrickColor::BrickAtlasMap();
			}


		};
	}
}