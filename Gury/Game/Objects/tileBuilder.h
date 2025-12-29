#pragma once

#include <G3DAll.h>

#include "../../Game/Objects/PVInstance/pvenums.h"

namespace RBX
{
	class TileBuilder
	{
	public:
		static void getSliceInformation(const Color4& color, NormalId face, SurfaceType surface, const Vector3& realSize, Vector2& sliceSize, Vector2& u, Vector2& v);

		static void appendTexCoordsXYWH(Array<Vector2>& texCoordsOut, Vector2 ru, Vector2 rv, Vector2 size, float width, float height);
		static void appendHelpedTexCoords(Array<Vector2>& texCoordsOut, Vector2 ru, Vector2 rv, Vector2 size, float w, float h, bool drawHorizontal);

		static void rbxAppendProductSlice(const Vector3& size, NormalId face, SurfaceType surface, Vector2 sliceSize, Vector2 u, Vector2 v, float cx, float cy, float xw, float yh, float worldY, float width, float height, Array<Vector3>& out, Array<Vector2>& texCoordsOut);
		static void rbxAppendRemainderSlice(NormalId face, SurfaceType surface, const Vector3& size, const Vector2& sliceSize, float rn0, float rn1, float rn2, float x, float y, float cx, float cy, float xw, float yh, float worldX, float worldY, float sliceW, float sliceH, float width, float height, Array<Vector3>& out, Array<Vector2>& texCoordsOut);
		static void generate_texture(const Color4& color, NormalId face, SurfaceType surface, const Vector2& sliceSize, float rn0, float rn1, float rn2, float x, float y, float cx, float cy, float xw, float yh, float worldX, float worldY, float sliceW, float sliceH, float width, float height, Array<Vector3>& out, Array<Vector2>& texCoordsOut);

		static void tile(const Color4& color, const Vector3& size, SurfaceType surface, NormalId face, int width, int height, float w, float y, float h, Vector2 u, Vector2 v, CoordinateFrame cframe, Array<Vector3>& out, Array<Vector2>& texCoordsOut);
	};
}