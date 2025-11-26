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

			float alpha, transparency, reflectance;
			Array<uint32> meshIndices;

			Render::Mesh* currentProxy;
			IRenderable* specialShape;

			void editMeshPosition(CoordinateFrame newPosition);
			bool inRenderEnvironment() const;

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

			void addToProxy(Render::Mesh* proxy);
			void removeFromProxy(Render::Mesh* proxy);

			void editGlobalProxyLocation();
			void editGlobalTexturedProxyLocation();

			/* renderface but with (preferably) different uv mapping */

			virtual void renderFaceFitForDecal(RenderDevice* rd, NormalId face) {};
			virtual void renderFaceFitForTexture(RenderDevice* rd, NormalId face, Vector2 StudsUV) {};

			~IRenderable() {
				meshIndices.clear();
				delete &meshIndices;
			}
			IRenderable() 
			{
				meshIndices = Array<uint32>();
				alpha = 0.0f;
				transparency = 0.0f;
				reflectance = 0.0f;
				currentProxy = 0;
				specialShape = 0;
			}
			RTTR_ENABLE(RBX::Instance);
		};

	}
}
