#pragma once

#include <G3DAll.h>

#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Rendering/renderMesh.h"

#include "instance.h"

namespace RBX
{
	class Decal;

	namespace Render
	{

		class IRenderable : 
			public RBX::Instance
		{
		public:

			bool unaffectedByLight, renderedLast;
			float alpha, transparency, reflectance;

			Render::Mesh* currentProxy;

			struct Face
			{
				Array<int> indices;
				Array<int> originIndices;
				Vector2 u, v;
				bool textured;
			};

			Table<NormalId, Face> vertexIndices;

			IRenderable* specialShape;

			void editMeshPosition(CoordinateFrame newPosition);
			virtual void removeFromRenderEnvironment();

			virtual void write() {};
			virtual void edit() {};
		
			virtual void writeSurfaces() {};

			virtual void write3DSurfaces() {};
			virtual void writeBrickFace(NormalId face, int unit, bool repeat = false) {};

			virtual void editBrickFace(NormalId face, int unit, bool repeat = false) {};

			virtual void render2D(RenderDevice* renderDevice) {} /* for rendering 2dly */
			virtual void renderAdornee(RenderDevice* renderDevice) {} /* for other world rendered things */

			virtual void regenerateRenderable() {} /* uhh */

			/* levels */

			void addVertexIndicesToProxy(Render::Mesh* proxy, Table<NormalId, Face> vIndices);
			void removeVertexIndicesFromProxy(Render::Mesh* proxy, Table<NormalId, Face> vIndices);

			void removeFaceFromProxy(Render::Mesh* proxy, Face face);
			void addFaceToProxy(Render::Mesh* proxy, Face face);

			void addToProxy(Render::Mesh* proxy);
			void removeFromProxy(Render::Mesh* proxy);

			void editGlobalProxyLocation();
			void editGlobalTexturedProxyLocation();

			/* renderface but with (preferably) different uv mapping */

			virtual void renderFaceFitForDecal(RenderDevice* rd, NormalId face) {};
			virtual void renderFaceFitForTexture(RenderDevice* rd, NormalId face, Vector2 StudsUV) {};

			virtual ~IRenderable() {}
			IRenderable() 
			{
			}
			RTTR_ENABLE(RBX::Instance);
		};

	}
}
