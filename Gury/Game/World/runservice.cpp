
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
    if (!isRunning)
    {
        RBX::Network::Player* localPlayer;
        localPlayer = RBX::Network::Players::get()->localPlayer;

        WorldScene::get()->saveStartPVs();
        Gurnel::get()->spawnWorld();

        RBX::StandardOut::print(RBX::MESSAGE_INFO, "RunService::run() diagnostic splash: %d primitives and %d bodies in world", Gurnel::get()->getPrimitivesInWorld(), Gurnel::get()->getBodiesInWorld());

        if (!hasStarted)
        {
            reset();
            hasStarted = 1;
        }

        if (localPlayer)
        {
            localPlayer->setAsController();
        }

        if (!isPaused) /* dont restart scripts on resume */
        {
            if (scriptContext)
            {
                scriptContext->runScripts();
            }
        }
        else
        {
            onResume();
        }

        isRunning = true;
        isPaused = false;
     
    }
}

void RBX::RunService::pause()
{
    stop();
    isPaused = true;
    onPause();
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
        shouldReset = 1;
        update();
        stop();
    }
}

void RBX::RunService::update()
{

    if (isRunning)
    {
        RBX::WorldScene::get()->updateSteppables();
        RBX::WorldScene::get()->updateSteppablesKernelly();

        for (int i = 0; i < 4; i++)
        {
            Gurnel::get()->step(0.02f, 2);
        }

        Gurnel::get()->afterStep();

    }

    /* reset pvs */

    if (shouldReset)
    {
        resetPvs();

        Gurnel::get()->markForReset(true);
        Gurnel::get()->afterStep();

        onReset();
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
    Instances scene = WorldScene::get()->getArrayOfObjects();
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