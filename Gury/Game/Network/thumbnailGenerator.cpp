
#include "../Gury/Application/appmanager.h"
#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Rendering/renderScene.h"
#include "../Gury/Game/Serializer/encode.h"

#include "thumbnailGenerator.h"

std::string RBX::ThumbnailGenerator::click(std::string fileType, int width, int height, bool hideSky)
{
	BinaryOutput output;

	doClick(fileType, width, height, hideSky, output);

	RBX::StandardOut::print(RBX::MESSAGE_INFO,
		"click %s, %d, %d, %s, successful",
		fileType.c_str(),
		width,
		height,
		hideSky ? "true" : "false");

	return base64_encode(output.getCArray(), output.length());
}

void RBX::ThumbnailGenerator::doClick(std::string fileType, int width, int height, bool imageServerView, BinaryOutput& out)
{	
	SkyRef sky;

	RBX::Render::RenderScene* view;
	RBX::Experimental::Application* application;

	RenderDevice* renderDevice;

	DummyWindow* dummyWindow;
	OSContext*  dummyContext;

	GImage::Format format;

	format = GImage::stringToFormat(fileType);
	if (format == GImage::UNKNOWN)
	{
		return;
	}

	dummyWindow = new DummyWindow(width, height);
	dummyContext = new OSContext(width, height, 3+imageServerView);

	application = RBX::RBXManager::get()->getApplication();
	view = RBX::Render::RenderScene::get();

	dummyContext->reallyMakeCurrent();

	renderDevice = new RenderDevice();
	renderDevice->init(dummyWindow->getWindow());

	renderDevice->pushState();
	renderDevice->setDepthTest((RenderDevice::DepthTest)(RenderDevice::DEPTH_GEQUAL | RenderDevice::DEPTH_LEQUAL));

	if (imageServerView)
	{
		application->getDatamodel()->workspace->setImageServerView();
	}
	else
	{
		sky = view->createSky(renderDevice);
	}

	renderDevice->setColorClearValue(Color4::CLEAR);

	view->oneFrame(renderDevice, application->getCamera(),
		sky);

	renderDevice->popState();

	if (imageServerView)
	{
		dummyContext->image.flipRGBAVertical(
			dummyContext->image._byte,
			dummyContext->image._byte,
			width, height);
	}
	else
	{
		dummyContext->image.flipRGBVertical(
			dummyContext->image._byte,
			dummyContext->image._byte,
			width, height);
	}

	dummyContext->image.encode(format, out);
	renderDevice->cleanup();

	delete dummyWindow;
}
