
#include "../Gury/Game/Rendering/renderScene.h"

#include "../Gury/Game/Rendering/scene.h"
#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Services/selection.h"

#include "../Gury/Game/Services/service.h"

#include "../Gury/Game/Rendering/diagnosticsWorldDrawer.h"
#include "../Gury/Studio/StudioTool.h"

#include "../Gury/Game/Rendering/renderMesh.h"

RBX::Render::RenderScene* rbx_view;

static TextureRef texture;

void RBX::Render::RenderScene::onWorkspaceDescendentAdded(RBX::Instance* descendent)
{
	RBX::Scene::get()->onWorkspaceDescendentAdded((RBX::Render::IRenderable*)descendent);
}

void RBX::Render::RenderScene::onWorkspaceDescendentRemoved(RBX::Instance* descendent)
{
	RBX::Scene::get()->onWorkspaceDescendentRemoved((RBX::Render::IRenderable*)descendent);
}

void RBX::Render::RenderScene::presetLighting()
{
	if (!readyForToneMap)
	{
		params = getEffectSettings()->toneMap->prepareLightingParameters(params);
		lighting = getEffectSettings()->toneMap->prepareLighting(lighting);
		readyForToneMap = true;
	}
}

void RBX::Render::RenderScene::turnOnLights(RenderDevice* device)
{
	int n = 1;
	Color3 ambientColor;

	presetLighting();

	ambientColor = (lighting->ambientBottom + lighting->ambientTop) / 2.2f;

	device->setSpecularCoefficient(Color3::white() * 0.9f);
	device->setColorClearValue(colorClearValue);

	device->setShininess(100);

	device->setLight(0, GLight::directional(params.lightDirection, params.lightColor * 0.9f, 1, 1));

	for (int i = 0; i < lighting->lightArray.size(); i++)
	{
		device->setLight(n++, lighting->lightArray[i]);
	}

	for (int i = 0; i < lighting->shadowedLightArray.size(); i++)
	{
		device->setLight(n++, lighting->shadowedLightArray[i]);
	}

	device->setAmbientLightColor(ambientColor);

	if (getEffectSettings()->_hemisphereLighting)
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
void RBX::Render::RenderScene::sendProxyGeometry(RenderDevice* renderDevice, RenderSurface* proxy, bool quadOrTriangle)
{
	if (proxy)
	{
		renderDevice->setPolygonOffset(proxy->polygonOffset);
		proxy->material->configureRenderDevice(renderDevice);

		/* Render quad or triangle levels */
		if (quadOrTriangle)
		{
			Mesh::sendGeometry(proxy->fullMesh->quad_level, renderDevice);
		}
		else
		{
			Mesh::sendGeometry(proxy->fullMesh->triangle_level, renderDevice);
		}

	}
}

void RBX::Render::RenderScene::renderWorldLevels(RenderDevice* renderDevice, bool quadOrTriangle)
{
	/// Generate texture (if changed)
	//TextureRef superTexture;
	//Render::TextureReserve* reserve = Render::TextureReserve::get();

	//reserve->generateSuperTexture();
	//superTexture = reserve->getSuperTexture();

	/// Send opaque proxy

	sendProxyGeometry(renderDevice, proxy, quadOrTriangle);

	/// TODO: Write reflection stuff
	sendProxyGeometry(renderDevice, reflectProxy, quadOrTriangle);

	/// Set transparency stuff then render transparent geometry

	renderDevice->pushState();

	renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

	//renderDevice->setTexture(2, superTexture);
	//sendProxyGeometry(renderDevice, texturedProxy);
	//renderDevice->setTexture(2, 0);

	sendProxyGeometry(renderDevice, transparentProxy, quadOrTriangle);

	renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);

	renderDevice->popState();
}

void RBX::Render::RenderScene::renderScene(RenderDevice* renderDevice)
{
	renderDevice->pushState();

	renderDevice->enableLighting();
	turnOnLights(renderDevice);

	/* render triangle level */

	Mesh::beginRender(renderDevice);

	renderWorldLevels(renderDevice, 0);

	Mesh::endRender(renderDevice);

	Mesh::beginRender(renderDevice);

	renderWorldLevels(renderDevice, 1);

	Mesh::endRender(renderDevice);

	renderDevice->disableLighting();

	renderDevice->popState();
}

void RBX::Render::RenderScene::renderNonGeometricInstances(RenderDevice* renderDevice)
{
	Instances sceneObjects = Scene::get()->sceneObjects;
	for (unsigned int i = 0; i < sceneObjects.size(); i++)
	{
		IRenderable* iRenderable = toInstance<IRenderable>(sceneObjects.at(i));
		iRenderable->plainRender(renderDevice);
	}
}

void RBX::Render::RenderScene::oneFrame(RenderDevice* renderDevice, Camera* projection, SkyRef sky)
{
	Datamodel* datamodel = RBX::Datamodel::get();

	//presetLighting();

	renderDevice->beginFrame();

	renderDevice->clear();

	renderDevice->setColorClearValue(datamodel->lighting->getClearColor());

	renderDevice->setCullFace(RenderDevice::CULL_BACK);
	renderDevice->setAlphaTest(RenderDevice::ALPHA_GREATER, 0.1);
	renderDevice->setDepthTest((RenderDevice::DepthTest)(RenderDevice::DEPTH_GEQUAL | RenderDevice::DEPTH_LEQUAL));

	renderDevice->setStencilConstant(0);
	renderDevice->setStencilClearValue(0);

	renderDevice->setShadeMode(RenderDevice::ShadeMode::SHADE_SMOOTH);

	renderDevice->setProjectionAndCameraMatrix(*projection->getCamera());

	if (!sky.isNull())
	{
		sky->render(params);
	}

	renderDevice->pushState();

	renderScene(renderDevice);

	Selection::get()->renderSelection(renderDevice);

	/* render other things */
	renderNonGeometricInstances(renderDevice);

	if (RBX::Studio::current_Tool)
	{
		RBX::Studio::current_Tool->doGraphics(renderDevice);
	}

	RBX::Diagnostics::get_Renderer()->render(renderDevice);

	renderDevice->popState();

	if (!effectSettings->toneMap.isNull())
	{
		effectSettings->toneMap->endFrame(renderDevice);
	}

	if (!sky.isNull())
	{
		sky->renderLensFlare(params);
	}

	renderDevice->push2D();

	datamodel->guiRoot->render(renderDevice);
	datamodel->message->render(renderDevice);

	Selection::get()->renderDragBox(renderDevice);
	Diagnostics::get_Renderer()->render2D(renderDevice);

	Mouse::get()->render(renderDevice);

	renderDevice->pop2D();

	renderDevice->endFrame();

}

RBX::Render::RenderScene* RBX::Render::RenderScene::get()
{
	if (!rbx_view)
	{
		rbx_view = new RBX::Render::RenderScene();
	}
	return rbx_view;
}
