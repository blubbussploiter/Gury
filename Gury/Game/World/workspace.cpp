
#include "../Gury/Game/Objects/part.h"
#include "../Gury/Game/Objects/model.h"

#include "../Gury/Game/World/runservice.h"

#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/Services/scriptcontext.h"
#include "../Gury/Game/Services/selection.h"

#include "../Gury/Application/appmanager.h"

#include "camera.h"
#include "workspace.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Workspace>("Workspace")
        .constructor<>()
        .property_readonly("CurrentCamera", &RBX::Workspace::getCurrentCamera)(rttr::metadata("Type", RBX::Data));
}

void RBX::Workspace::wakeUpModels()
{
    RBX::Instances* children;
    children = getChildren();

    for (unsigned int i = 0; i < children->size(); i++)
    {
        RBX::Instance* child;
        RBX::ModelInstance* model;
        child = children->at(i);
        model = dynamic_cast<RBX::ModelInstance*>(child);
        if (model)
        {
            model->buildJoints();
            model->createController();
        }
    }
}

void RBX::Workspace::playAllSounds()
{
    Instances* sounds = new Instances();
    getAllInstancesOfClass(getChildren(), sounds, "Sound");

    for (size_t i = 0; i < sounds->size(); i++)
    {
        Sound* sound = toInstance<Sound>(sounds->at(i));
        if (sound)
        {
            sound->play();
        }
    }
}

void RBX::Workspace::getPVInstances(RBX::Instances* instances, RBX::Instances* pvs)
{
    for (unsigned int i = 0; i < instances->size(); i++)
    {
        RBX::Instance* child = instances->at(i);

        if (RBX::IsA<RBX::PartInstance>(child))
            pvs->push_back(child);

        getPVInstances(child->getChildren(), pvs);
    }
}

void RBX::Workspace::getAllInstancesOfClass(RBX::Instances* instances, RBX::Instances* instancesOut, std::string className)
{
    for (unsigned int i = 0; i < instances->size(); i++)
    {
        RBX::Instance* child = instances->at(i);

        if (child->className == className)
            instances->push_back(child);

        getAllInstancesOfClass(child->getChildren(), instancesOut, className);
    }
}

void RBX::Workspace::workspaceDescendentAdded(Instance* _this, Instance* descendent)
{
    RBX::RunService::get()->onWorkspaceDescendentAdded(descendent);
    RBX::ScriptContext::get()->onWorkspaceDescendentAdded(descendent);
    RBX::Render::RenderScene::get()->onWorkspaceDescendentAdded(descendent);
}

void RBX::Workspace::workspaceDescendentRemoved(Instance* _this, Instance* descendent)
{
    RBX::Render::RenderScene::get()->onWorkspaceDescendentRemoved(descendent);
    RBX::ScriptContext::get()->onWorkspaceDescendentRemoved(descendent);
    RBX::SelectionService::get()->deselect(descendent);
}

RBX::Instances* RBX::Workspace::getPVInstancesArray()
{
    RBX::Instances* instances = new RBX::Instances();
    getPVInstances(Workspace::get()->children, instances);
    return instances;
}

bool RBX::Workspace::setImageServerView()
{
    size_t numChildren;
    Instances* children;

    children = getChildren();
    numChildren = children->size();

    if (numChildren)
    {
        for (size_t i = 0; i < numChildren; i++)
        {
            RBX::ModelInstance* model = dynamic_cast<RBX::ModelInstance*>(children->at(i));
            if (model)
            {
                PVInstance* primaryPart = model->getPrimaryPart();
                if (primaryPart)
                {
                    CoordinateFrame cframe = primaryPart->getCFrame();
                    getCurrentCamera()->setImageServerViewNoLerp(cframe);
                }
            }
        }
    }

    return true;
}

RBX::Extents RBX::Workspace::computeCameraOwnerExtents()
{
    RBX::PartInstance* primaryPart;
    primaryPart = getPrimaryPart();

    if (primaryPart)
    {
        return primaryPart->getWorldExtents();
    }

    return RBX::Extents();
}

RBX::Workspace* RBX::Workspace::get()
{
    return RBX::Datamodel::get()->workspace;
}

/* hacky method */

void RBX::Workspace::createCurrentCamera()
{
    if (!currentCamera)
    {
        RBX::StandardOut::print(RBX::MESSAGE_INFO, "Workspace::getCurrentCamera() creating current camera");
        currentCamera = new Camera();
        currentCamera->updateView();
        currentCamera->setParent(this);
    }
}

RBX::Camera* RBX::Workspace::getCurrentCamera()
{
    createCurrentCamera();
    return currentCamera;
}