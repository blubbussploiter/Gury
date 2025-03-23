
#include "../Gury/Studio/pch.h"
#include "../Gury/Studio/MainFrm.h"

#include "../Gury/Game/Network/thumbnailGenerator.h"
#include "../Gury/Game/Lua/yieldingthreads.h"
#include "../Gury/Game/Objects/controller.h"
#include "../Gury/Game/Network/Player/players.h"

#include "../Gury/Application/appmanager.h"
#include "../Gury/Game/Serializer/serializer.h"
#include "../Gury/Game/Services/stdout.h"

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
    RBX::StandardOut::print(MESSAGE_INFO, "DataModel loading %s", contentId.c_str());
    RBX::Serializer::load(contentId);

    JointsService::get()->buildGlobalJoints();

}

void RBX::Datamodel::saveContent(std::string contentId)
{
    RBX::StandardOut::print(MESSAGE_INFO, "DataModel saving %s", contentId.c_str());
    RBX::Serializer::save(contentId);
}

void RBX::Datamodel::close()
{
    RBX::StandardOut::print(MESSAGE_INFO, "DataModel::close()");
    RBX::ScriptContext::get()->close();
    RBX::Scene::get()->close();
    Gurnel::get()->cleanup();
    RBX::Log::cleanup();
    emptyExplorerWindow();
}

void RBX::Datamodel::descendentAdded(RBX::Instance* i)
{
    if (i->parent != get())
    {
        CMainFrame::mainFrame->m_wndClassView.AddInstance(i, 0);
    }
}

void RBX::Datamodel::descendentRemoved(RBX::Instance* i)
{
    get()->removeFromExplorerWindow(i);
}

void RBX::Datamodel::fillExplorerWindow()
{
    addToExplorerWindow(workspace);
    addToExplorerWindow(players);
    addToExplorerWindow(lighting);
    addToExplorerWindow(soundService);
    addToExplorerWindow(jointService);
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

void RBX::Datamodel::open()
{
    guiRoot = new Gui::GuiRoot();
    workspace = new Workspace();
    runService = new RunService();
    renderScene = new Render::RenderScene();
    lighting = new Lighting();
    scene = new Scene();
    controllerService = new ControllerService();
    thumbnailGenerator = new ThumbnailGenerator();
    scriptContext = new ScriptContext();
    soundService = new SoundService();
    players = new RBX::Network::Players();   
    jointService = new JointsService();
    selectionService = new SelectionService();
    runService->scriptContext = scriptContext;
    yieldingThreads = new Lua::YieldingThreads(scriptContext);
    physicsKernel = new Gurnel();
    globalMesh = new Render::Mesh();

    fillExplorerWindow();
}

void RBX::Datamodel::step(double deltaTime)
{
    runService->heartbeat(deltaTime);
    yieldingThreads->resume(deltaTime);
}