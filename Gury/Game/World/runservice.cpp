
#include <queue>

#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/Rendering/scene.h"

#include "../Gury/Game/Services/scriptcontext.h"
#include "../Gury/Game/Services/service.h"
#include "../Gury/Kernel/jointsservice.h"

#include "../Gury/Game/Objects/model.h"
#include "../Gury/Game/Network/Player/players.h"

#include "runservice.h"
#include "workspace.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::RunService>("RunService")
        .constructor<>()
        .method("pause", &RBX::RunService::stop)
        .method("run", &RBX::RunService::run);
}

void RBX::RunService::run()
{
    if (isRunning) return;

    RBX::Network::Player* localPlayer;
    localPlayer = RBX::Network::Players::get()->localPlayer;

    Scene::get()->saveStartPVs();
    Gurnel::get()->spawnWorld();

    if (!hasStarted)
    {
        reset();
        hasStarted = 1;
    }

    if (localPlayer)
    {
        localPlayer->setAsController();
    }

    if (scriptContext)
    {
        scriptContext->runScripts();
    }

    isRunning = true;
    isPaused = false;
}

void RBX::RunService::pause()
{
    stop();
    isPaused = true;
}

void RBX::RunService::stop()
{
    isRunning = false;
    isPaused = false;
}

void RBX::RunService::reset()
{
    if (!hasStarted) /* to do : make this dependent on the functions below! this doesnt check for new objects / objects added after run */
    {
        JointsService::get()->buildGlobalJoints();
        JointsService::get()->buildConnectors();
    }
    else
    {
        stop();
        shouldReset = 1;
    }
}

void RBX::RunService::update()
{

    if (isRunning)
    {
        RBX::Scene::get()->updateSteppables();
        RBX::Scene::get()->updateSteppablesKernelly();

        for (int i = 0; i < 4; i++)
        {
            Gurnel::get()->step(0.03f, 4);
        }

        Gurnel::get()->afterStep();

    }

    /* reset pvs */

    if (shouldReset)
    {
        resetPvs();
        shouldReset = 0;
    }

}

void RBX::RunService::heartbeat(float deltaTime)
{
    this->deltaTime = deltaTime;
    update();
}

void RBX::RunService::updateSteppers()
{
    for (unsigned int i = 0; i < steppers->size(); i++)
    {
        steppers->at(i)->onStep();
    }
}

void RBX::RunService::resetPvs()
{
    Instances scene = Scene::get()->getArrayOfObjects();
    for (unsigned int i = 0; i < scene.size(); i++)
    {
        PVInstance* pv = toInstance<PVInstance>(scene.at(i));
        if (pv)
        {
            if (pv->getBody())
            {
                pv->getBody()->setDisabled(false);
            }
            pv->resetPV();
        }
    }
}

void RBX::RunService::onWorkspaceDescendentAdded(RBX::Instance* descendent)
{

}

RBX::RunService* RBX::RunService::get()
{
    return RBX::Datamodel::get()->runService;
}