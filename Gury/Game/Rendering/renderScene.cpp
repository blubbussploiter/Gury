
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Services/selection.h"

#include "../Gury/Game/Services/service.h"
#include "../Gury/Studio/StudioTool.h"

#include "renderScene.h"

#include "scene.h"
#include "diagnosticsWorldDrawer.h"

#include "renderMesh.h"

static TextureRef texture;

void RBX::Render::RenderScene::onWorkspaceDescendentAdded(RBX::Instance* descendent)
{
	RBX::WorldScene::get()->onWorkspaceDescendentAdded((RBX::Render::IRenderable*)descendent);
}

void RBX::Render::RenderScene::onWorkspaceDescendentRemoved(RBX::Instance* descendent)
{
	RBX::WorldScene::get()->onWorkspaceDescendentRemoved((RBX::Render::IRenderable*)descendent);
}

void RBX::Render::RenderScene::presetLighting()
{
	RBX::Lighting* RBXLighting = RBX::Lighting::get();

	lighting = RBXLighting->lighting;
	params = *RBXLighting->params;

	if (!readyForToneMap)
	{
		params = getEffectSettings()->toneMap->prepareLightingParameters(params);
		lighting = getEffectSettings()->toneMap->prepareLighting(lighting);
		readyForToneMap = true;
	}
}

void RBX::Render::RenderScene::turnOnLights(RenderDevice* device)
{
	float spotDamper = 0.89f;
	float ambientDamper = 0.5f;

	int n = 1;

	Color3 ambientColor;
	EffectSettings* effectSettings = getEffectSettings();

	presetLighting();

	effectSettings->_hemisphereLighting = true;
	ambientColor = (lighting->ambientBottom + lighting->ambientTop) * 0.5f;
	
	device->setColorClearValue(colorClearValue);
	device->setColor(Color3::white());

	device->setLight(0, GLight::directional(params.lightDirection, params.lightColor * 0.9f));

	for (int i = 0; i < lighting->lightArray.size(); i++)
	{
		device->setLight(n++, lighting->lightArray[i]);
	}

	for (int i = 0; i < lighting->shadowedLightArray.size(); i++)
	{
		device->setLight(n++, lighting->shadowedLightArray[i]);
	}

	device->setAmbientLightColor(ambientColor);

	if (effectSettings->_hemisphereLighting)
	{
		if (lighting->ambientBottom != ambientColor)
		{
			Color3 ambient = lighting->ambientBottom - ambientColor;
			device->setLight(n++, GLight::directional(Vector3(0.4f, -1.0f, 0.1f), ambient, 0, 1));
		}

		if (lighting->ambientTop != ambientColor)
		{
			Color3 ambient = lighting->ambientTop - ambientColor;
			device->setLight(n++, GLight::directional(Vector3::unitY(), ambient, 0, 1));
		}
	}
}
void RBX::Render::RenderScene::sendProxyGeometry(RenderDevice* renderDevice, RenderSurface* proxy)
{
	if (proxy)
	{
		proxy->material->configureRenderDevice(renderDevice);

		Mesh::sendGeometry(proxy->fullMesh->triangle_level, renderDevice);

	}
}

void RBX::Render::RenderScene::renderWorldLevels(RenderDevice* renderDevice)
{
	/// Send opaque proxy

	renderDevice->setTexture(0, TextureReserve::get()->getSuperTexture());

	sendProxyGeometry(renderDevice, proxy);

	/// TODO: Write reflection stuff
	sendProxyGeometry(renderDevice, reflectProxy);

	/// Set transparency stuff then render transparent geometry

	renderDevice->pushState();

	renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

		renderDevice->setTexture(0, TextureReserve::get()->getSuperTexture());

		sendProxyGeometry(renderDevice, transparentProxy);

	renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);

	renderDevice->popState();
}

void RBX::Render::RenderScene::renderScene(RenderDevice* renderDevice)
{
	renderDevice->pushState();

	renderDevice->enableLighting();
	turnOnLights(renderDevice);

	Mesh::beginRender(renderDevice);

		renderWorldLevels(renderDevice);

	Mesh::endRender(renderDevice);

	renderDevice->disableLighting();

	renderDevice->popState();
}

void RBX::Render::RenderScene::renderNonGeometricInstances(RenderDevice* renderDevice)
{
	Instances sceneObjects = WorldScene::get()->sceneObjects;
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		IRenderable* iRenderable = toInstance<IRenderable>(sceneObjects.at(i));
		iRenderable->renderAdornee(renderDevice);
	}
}

void RBX::Render::RenderScene::render2DInstances(RenderDevice* renderDevice)
{
	Instances sceneObjects = WorldScene::get()->sceneObjects;
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		IRenderable* iRenderable = toInstance<IRenderable>(sceneObjects.at(i));
		iRenderable->render2D(renderDevice);
	}
}

void RBX::Render::RenderScene::oneFrame(RenderDevice* renderDevice, Camera* projection, SkyRef sky)
{

	Datamodel* datamodel = RBX::Datamodel::get();

	if (datamodel->loaded)
	{
		/* Generate / regenerate world */

		TextureReserve::get()->generateSuperTexture();
		WorldManager::get()->step();

		renderDevice->beginFrame();

		renderDevice->clear();

		renderDevice->setColorClearValue(datamodel->lighting->getClearColor());

		renderDevice->setCullFace(RenderDevice::CULL_BACK);
		renderDevice->setAlphaTest(RenderDevice::ALPHA_GREATER, 0.1);

		renderDevice->setDepthWrite(true);
		renderDevice->setDepthTest((RenderDevice::DepthTest)(RenderDevice::DEPTH_LESS));

		renderDevice->setStencilConstant(0);
		renderDevice->setStencilClearValue(0);

		renderDevice->setShadeMode(RenderDevice::ShadeMode::SHADE_SMOOTH);

		renderDevice->setProjectionAndCameraMatrix(*projection->getCamera());

		renderDevice->pushState();

		if (!sky.isNull())
		{
			sky->render(params);
		}

		renderScene(renderDevice);

		SelectionService::get()->renderSelection(renderDevice);

		if (RBX::Studio::current_Tool)
		{
			RBX::Studio::current_Tool->doGraphics(renderDevice);
		}

		RBX::Diagnostics::get_Renderer()->render(renderDevice);

		renderDevice->popState();

		/* render other things */

		renderNonGeometricInstances(renderDevice);

		if (!effectSettings->toneMap.isNull())
		{
			effectSettings->toneMap->endFrame(renderDevice);
		}

		if (!sky.isNull())
		{
			sky->renderLensFlare(params);
		}

		renderDevice->push2D();

		render2DInstances(renderDevice);

		datamodel->render(renderDevice);
		datamodel->guiRoot->render(renderDevice);

		SelectionService::get()->renderDragBox(renderDevice);
		Diagnostics::get_Renderer()->render2D(renderDevice);

		Mouse::get()->render(renderDevice);

		renderDevice->pop2D();

		renderDevice->endFrame();

	}

}

RBX::Render::RenderScene* RBX::Render::RenderScene::get()
{
	return RBX::Datamodel::get()->renderScene;
}
