#pragma once

#include <G3DAll.h>

/*

* -- DEAREST ROBLOX REVERSE ENGINEERS: AN APOLOGY -- *

* Gury's RBX::Render::Materials and Levels behave NOTHING like ROBLOX's respected classes
* although the members' names and functions' names are mostly 1:1, they don't function ENTIRELY similar
* I would like to dearly apologize if I am defaming these wonderous classes' names with my versions of these functions.

Sincerely, Eric Nutter (whirlpool)

*/

namespace RBX
{
	namespace Render
	{
		class Material
		{
		public:

			class Level
			{
			public:
				bool emptyLevel;
				Color3 mColor;
				Color3 mSpecular;
				float mShiny;
				float mReflect;
				float mTransparent;
			public:

				void configureRenderDevice(RenderDevice* renderDevice);
				void baseTexture(RenderDevice* renderDevice);
				void matte(RenderDevice* renderDevice);

				Level()
				{
					emptyLevel = 1;
				}

				Level(Color3 mColor, Color3 mSpecular, float mShiny, float mReflect, float mTransparent) :
					mColor(mColor), mSpecular(mSpecular), mShiny(mShiny), mReflect(mReflect), mTransparent(mTransparent)
				{
					if (mTransparent > 0.0)
						mReflect = 0.0;
				}
			};

			G3D::Array<Level*> levels;

			void appendLevel(Color3 color, Color3 specular, float shiny, float reflect, float transparent);
			void appendEmptyLevel();

			//// GURY STUFF 

			Level* getVeryTransparentLevel();
			Level* getVeryReflectantLevel();
			Level* getVeryOpaqueLevel();

			bool veryTransparent(int level);
			bool veryReflectant(int level);
			bool veryOpaque(int level);

			//// END OF GURY STUFF

		};
	}
}