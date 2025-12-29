
#include "../instanceGarbageCollector.h"

#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/MainFrm.h"

#include "../Gury/Game/Network/thumbnailGenerator.h"
#include "../Gury/Game/Lua/yieldingthreads.h"
#include "../Gury/Game/Objects/controller.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Application/appmanager.h"
#include "../Gury/Game/Serializer/serializer.h"

#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Game/Services/Hopper.h"

#include "../Gury/Game/World/runservice.h"
#include "../Gury/Kernel/jointsservice.h"

#include "../Gury/Game/Services/soundservice.h"
#include "../Gury/Game/Services/selection.h"

#include "../Gury/Game/Rendering/renderScene.h"

#include "../Gury/Main/framework.h"

#include "workspace.h"
#include "lighting.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Datamodel>("Datamodel")
         .constructor<>()
         .method("SetMessageBrickCount", &RBX::Datamodel::setMessageBrickCount)
         .method("load", &RBX::Datamodel::loadContent);
}

RBX::Datamodel* RBX::Datamodel::get()
{
    RBX::Experimental::Application* application = RBX::RBXManager::get()->getApplication();
    if (application)
    {
        return application->getDatamodel();
    }
    return 0;
}

void RBX::Datamodel::loadContent(std::string contentId)
{
    loaded = false;

    RBX::StandardOut::print(MESSAGE_INFO, "DataModel loading %s", contentId.c_str());
    RBX::Serializer::load(contentId);

    Gurnel::get()->afterStep();
    JointsService::get()->buildGlobalJoints();

    loaded = true;

    RBX::StandardOut::print(MESSAGE_INFO, "DataModel::loadContent diagnostic splash: %d scripts, %d bricks, %d joints, %d primitives, %d bodies, %d textures", scriptContext->getNumScripts(), workspace->getNumBricks(), jointService->getNumConnectors(), Gurnel::get()->getPrimitivesInWorld(), Gurnel::get()->getBodiesInWorld(), Render::TextureReserve::get()->getNumTextures());
}

void RBX::Datamodel::saveContent(std::string contentId)
{
    RBX::StandardOut::print(MESSAGE_INFO, "DataModel saving %s", contentId.c_str());
    RBX::Serializer::save(contentId);
}

void RBX::Datamodel::close()
{
    RBX::StandardOut::print(MESSAGE_INFO, "DataModel::close()");
    RBX::ScriptContext::get()->closeState();
    RBX::WorldScene::get()->close();
    RBX::Log::close();
    Gurnel::get()->close();
    emptyExplorerWindow();
    workspace->clearAllChildren();
    guiRoot->clearAllChildren();
}

void RBX::Datamodel::descendentAdded(Instance* _this, Instance* i)
{
    if (i->parent != get())
    {
        CMainFrame::mainFrame->m_wndClassView.AddInstance(i, 0);
    }
}

void RBX::Datamodel::descendentRemoved(Instance* _this, Instance* i)
{
    get()->removeFromExplorerWindow(i);
}

void RBX::Datamodel::fillExplorerWindow()
{
    addToExplorerWindow(workspace);
    addToExplorerWindow(players);
    addToExplorerWindow(lighting);
    addToExplorerWindow(soundService);
    addToExplorerWindow(hopper);
}

void RBX::Datamodel::emptyExplorerWindow()
{
    CMainFrame::mainFrame->m_wndClassView.ClearInstances();
}

void RBX::Datamodel::addToExplorerWindow(RBX::Instance* i)
{
    CMainFrame::mainFrame->m_wndClassView.AddInstance(i);
}

void RBX::Datamodel::removeFromExplorerWindow(RBX::Instance* i)
{
    CMainFrame::mainFrame->m_wndClassView.RemoveInstance(i);
}

void RBX::Datamodel::render(RenderDevice* renderDevice)
{
    if (!gameHint->text.empty()) {
        gameHint->render(renderDevice);
    }

    /* Single message system didn't work out to well.. just render from workspace */
    //if (!gameMessage->text.empty()) {
    //    gameMessage->render(renderDevice);
    //}
}

void RBX::Datamodel::open()
{
    guiRoot = new Gui::GuiRoot();
    players = new RBX::Network::Players();
    hopper = new Hopper();
    workspace = new Workspace();
    runService = new RunService();
    renderScene = new Render::RenderScene();
    lighting = new Lighting();
    scene = new WorldScene();
    controllerService = new ControllerService();
    thumbnailGenerator = new ThumbnailGenerator();
    scriptContext = new ScriptContext();
    soundService = new SoundService();
    jointService = new JointsService();
    selectionService = new SelectionService();
    runService->scriptContext = scriptContext;
    yieldingThreads = new Lua::YieldingThreads(scriptContext);
    physicsKernel = new Gurnel();
    globalMesh = new Render::Mesh();

    loaded = true;

    onOpened();

    fillExplorerWindow();
}

void RBX::Datamodel::step(double deltaTime)
{
    InstanceGarbageCollector::get()->step();
    runService->heartbeat(deltaTime);
    //->resume(deltaTime);
}