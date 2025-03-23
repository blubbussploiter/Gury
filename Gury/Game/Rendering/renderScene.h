#pragma once

#include "../Gury/Game/World/camera.h"

#include "scene.h"

#include "effectsetting.h"
#include "shadows.h"

#include "renderMesh.h"
#include "renderSurface.h"

#include "surfaceFactory.h"
#include "materialFactory.h"

namespace RBX
{
	namespace Render
	{

		class RenderScene : public Instance
		{
		private:

			bool readyForToneMap;
			bool graphicsInitialized;

		public:

			Render::EffectSettings* effectSettings;
			Shadows* shadows;

			LightingRef lighting;
			LightingParameters params;

			Color3 colorClearValue;
			SkyRef sky;

			Render::RenderSurface* proxy;
			Render::RenderSurface* reflectProxy;
			Render::RenderSurface* transparentProxy;
			Render::RenderSurface* texturedProxy;

			void onWorkspaceDescendentAdded(RBX::Instance* descendent);
			void onWorkspaceDescendentRemoved(RBX::Instance* descendent);

			void presetLighting();
			void turnOnLights(RenderDevice* rd);

			void renderWorldLevels(RenderDevice* rd);
			void renderScene(RenderDevice* rd);

			void renderNonGeometricInstances(RenderDevice* rd);

			void sendProxyGeometry(RenderDevice* renderDevice, RenderSurface* proxy);

			void oneFrame(RenderDevice* rd, Camera* projection, SkyRef sky);

			Render::EffectSettings* getEffectSettings()
			{
				if (!effectSettings)
				{
					effectSettings = new Render::EffectSettings();
					effectSettings->_hemisphereLighting = 1;
					effectSettings->toneMap->setEnabled(false);
				}
				return effectSettings;
			}

			void renderSky(SkyRef _sky, LightingParameters parameters)
			{
				if (sky.isNull())
				{
					sky = _sky;
				}
				sky->render(parameters);
			}

			SkyRef createSky(RenderDevice* rd)
			{
				return Sky::create(rd, ConFileInPath("\\content\\sky\\"));
			}

			RenderScene()
			{
				lighting = G3D::Lighting::create();
				params = G3D::LightingParameters();
				shadows = new Shadows();

				/* create proxies

					gury does this way different than roblox.

				*/

				Material* factory = MaterialFactory<PartInstance>::factory()->getMaterial();
				
				proxy = new Render::RenderSurface(factory->levels[0]);
				reflectProxy = new Render::RenderSurface(factory->levels[1]);
				transparentProxy = new Render::RenderSurface(factory->levels[2]);
				texturedProxy = new Render::RenderSurface(factory->levels[3]);

			}

			static RenderScene* get();
		};
	}
}