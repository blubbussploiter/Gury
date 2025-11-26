#pragma once

#include "../Gury/Game/Objects/decal.h"

namespace RBX
{
	class Texture : public RBX::Decal
	{
	public:

		Vector2 studsPerTile;

		void render(RenderDevice* rd, RBX::Render::IRenderable* p); /* render with renderFace instead of renderFaceFitForDecal */

		float getStudsPerTileU() { return studsPerTile.x; }
		float getStudsPerTileV() { return studsPerTile.y; }
		void setStudsPerTileU(float u) { studsPerTile.x = u; }
		void setStudsPerTileV(float v) { studsPerTile.y = v; }

		Texture()
		{
			wrapMode = G3D::Texture::WrapMode::TILE;
			setName("Texture");
			setClassName("Texture");
		}

		RTTR_ENABLE(RBX::Decal);

	};
}