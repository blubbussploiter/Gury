#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Lua/singletonType.h"

namespace RBX
{
	extern GImage getSurfaceFromFile(RBX::SurfaceType s);

	class SurfaceFactory : public Singleton<SurfaceFactory>
	{
	public:

		bool dirty;

		static Table<SurfaceType, int> surfaces;
		Table<std::string, int> standardBrickHashes; /* for clones of bricks - dont overpopulate the atlas (first key: hash, second : bricktextures index in the atlas) */
		Array<PVInstance*> bricks; /* for regenerating uvs */

		Vector2 getFaceBounds(NormalId face, Vector3 brickSize);
		Vector2 getFaceSizePx(NormalId face, Vector3 brickSize);
		Vector2 getFacePositionRelative(NormalId face, Vector3 brickSize);
		Vector2 getTextureDimensions(Shape shape, Vector3 brickSize);

		std::string generateHash(Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom);

		void pasteFaceOnBrickTexture(GImage& bimage, GImage surfaceImage, NormalId face, Vector3 brickSize);

		void allocateBrickTexture(Shape shape, Vector3 brickSize, GImage& bimage);
		int generateBrickTexture(PVInstance* pv, Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom);
		void editBrickTexture(PVInstance* pv, int textureIndex, Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom);
		void removeBrickTexture(PVInstance* pv);

		/* re-edit the guys */
		void regen(PVInstance* current = 0);

		void pasteSurface(NormalId face, SurfaceType surface, Color4 color, Vector3 brickSize, GImage& image);
		void pasteSurfaces(GImage& brickTexture, Shape shape, Vector3 brickSize, Color4 brickColor, SurfaceType front, SurfaceType back, SurfaceType left, SurfaceType right, SurfaceType top, SurfaceType bottom);

	};
}