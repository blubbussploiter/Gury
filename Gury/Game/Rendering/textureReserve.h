#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Lua/singletonType.h"

namespace RBX
{
	namespace Render
	{

		class TextureReserve : public Singleton<TextureReserve>
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
			};

		private:

			int lastSize;

			Table<int, GImage*> bindedTextures;
			Table<int, TexturePositionalInformation> superTexturePositions;
			TextureRef guryWorldTexture; GImage superTextureData;

		public:

			Table<int, GImage*> getBindedTextures()
			{
				return bindedTextures;
			}

			int bindTexture(GImage texture)
			{
				int idx = bindedTextures.size();
				bindedTextures.set(idx, new GImage(texture));
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
				TexturePositionalInformation position = TexturePositionalInformation();
				if (superTexturePositions.containsKey(index))
				{
					return superTexturePositions[index];
				}
			}

			/* Remove all attached texCoords from this textureUnit */

			void unbindTexture(int texture)
			{
				//bindedTextures.erase(bindedTextures.begin() + texture);
			}

			TextureRef getSuperTexture()
			{
				return guryWorldTexture;
			}

			GImage getSuperTextureData()
			{
				return superTextureData;
			}

			Vector2 calculateSuperTextureDimensions();

			Array<Vector2> calculateTextureUV(int textureIndex);

			void generateSuperTexture();

		};
	}
}