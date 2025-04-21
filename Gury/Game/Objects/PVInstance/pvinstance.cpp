#include "../Gury/Game/Objects/instance.h"

#include "../Gury/Game/World/lighting.h"

#include "../Gury/Game/rbxmath.h"
#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/Objects/mesh.h"
#include "../Gury/Game/World/ray.h"

#include "../../../Kernel/jointsservice.h"

#include "pvinstance.h"
#include "pvenums.h"

#define CROSS Color3(0.5f, 0.5f, 0.5f)

RTTR_REGISTRATION
{
    rttr::registration::class_ <RBX::PVInstance>("PVInstance")
         .constructor<>()
         .property("Anchored", &RBX::PVInstance::getAnchored, &RBX::PVInstance::setAnchored)(rttr::metadata("Type", RBX::Behavior))
         .property("CanCollide", &RBX::PVInstance::getCanCollide, &RBX::PVInstance::setCanCollide)(rttr::metadata("Type", RBX::Behavior))
         .property("Locked", &RBX::PVInstance::getLocked, 
             &RBX::PVInstance::setLocked)(rttr::metadata("Type", RBX::Behavior))
         .property("Position", &RBX::PVInstance::getPosition, &RBX::PVInstance::setPosition)(rttr::metadata("Type", RBX::Data), rttr::metadata("Nonserializable", false))
         .property("Velocity", &RBX::PVInstance::getVelocity, &RBX::PVInstance::setVelocity)(rttr::metadata("Type", RBX::Data))
         .property("RotVelocity", &RBX::PVInstance::getRotVelocity, &RBX::PVInstance::setRotVelocity)(rttr::metadata("Type", RBX::Data))
         .property("Color", &RBX::PVInstance::getColor, &RBX::PVInstance::setColor4)(rttr::metadata("Nonserializable", false))
         .property("CFrame", &RBX::PVInstance::getCFrame, &RBX::PVInstance::setCFrame)
         .property("CoordinateFrame", &RBX::PVInstance::getCFrame, &RBX::PVInstance::setCFrame)(rttr::metadata("Nonserializable", false))
         .property("Elasticity", &RBX::PVInstance::getElasticity, &RBX::PVInstance::setElasticity)(rttr::metadata("Type", RBX::Part))
         .property("Friction", &RBX::PVInstance::getFriction, &RBX::PVInstance::setFriction)(rttr::metadata("Type", RBX::Part))
         .property("Shape", &RBX::PVInstance::getShape, &RBX::PVInstance::setShape)(rttr::metadata("Type", RBX::Part))
         .property("shape", &RBX::PVInstance::getShape, &RBX::PVInstance::setShape)(rttr::metadata("Nonserializable", false))
         .property("size", &RBX::PVInstance::getSizeExternal, &RBX::PVInstance::setSize)(rttr::metadata("Type", RBX::Part))
         .property("Size", &RBX::PVInstance::getSizeExternal, &RBX::PVInstance::setSize)(rttr::metadata("Nonserializable", false))
         .property("FrontSurface", &RBX::PVInstance::getFrontSurface, &RBX::PVInstance::setFrontSurface)(rttr::metadata("Type", RBX::Surface))
         .property("BackSurface", &RBX::PVInstance::getBackSurface, &RBX::PVInstance::setBackSurface)(rttr::metadata("Type", RBX::Surface))
         .property("TopSurface", &RBX::PVInstance::getTopSurface, &RBX::PVInstance::setTopSurface)(rttr::metadata("Type", RBX::Surface))
         .property("BottomSurface", &RBX::PVInstance::getBottomSurface, &RBX::PVInstance::setBottomSurface)(rttr::metadata("Type", RBX::Surface))
         .property("RightSurface", &RBX::PVInstance::getRightSurface, &RBX::PVInstance::setRightSurface)(rttr::metadata("Type", RBX::Surface))
         .property("LeftSurface", &RBX::PVInstance::getLeftSurface, &RBX::PVInstance::setLeftSurface)(rttr::metadata("Type", RBX::Surface))
         .property("rawFormFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)(rttr::metadata("Nonserializable", false))
         .property("formFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)(rttr::metadata("Nonserializable", false))
         .property("FormFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)
         //.property("FormFactor", &RBX::PVInstance::getFormFactor, &RBX::PVInstance::setFormFactor)
         .property("Transparency", &RBX::PVInstance::getFauxTransparency, &RBX::PVInstance::setTransparency)(rttr::metadata("Type", RBX::Appearance));
}
void RBX::PVInstance::write()
{
    if (specialShape)
    {
        specialShape->write();
    }
    else
    {
        switch (shape)
        {
        case Block:
        {
            /* in order 0 - 5*/

            writeBrickFace(Right, -1);
            writeBrickFace(Top, -1);
            writeBrickFace(Back, -1);
            writeBrickFace(Left, -1);
            writeBrickFace(Bottom, -1);
            writeBrickFace(Front, -1);

            writeSurfaces();

            break;
        }
        case Ball:
        { 
            sphereRadius = size.y;
            writeBall();
            break;
        }
        case Cylinder:
        {
            //RBX::Primitives::drawCylinder(d, this);
            break;
        }
        }
        editGlobalProxyLocation();
    }
}

void RBX::PVInstance::edit()
{
    if (specialShape)
    {
        specialShape->edit();
    }
    else
    {
        switch (shape)
        {
        case Block:
        {
            editBrickFaces();
            break;
        }
        case Ball:
        {
            sphereRadius = size.y;
            editBall();
            break;
        }
        }
        editGlobalProxyLocation();
    }
}

void RBX::PVInstance::onMeshAdded(Instance* _this, Instance* child)
{
    if (child && IsA<Render::SpecialMesh>(child))
    {
        Render::SpecialMesh* mesh = toInstance<Render::SpecialMesh>(child);
        Render::IRenderable* parent = toInstance<Render::IRenderable>(child->parent);

        if (parent)
        {
            RBX::StandardOut::print(RBX::MESSAGE_INFO,
                "Write new %s to %s", child->name.c_str(), parent->name.c_str());
            parent->removeFromRenderEnvironment();
            parent->specialShape = mesh;
            mesh->removeFromRenderEnvironment();
            parent->write();
        }
    }
}

void RBX::PVInstance::onMeshRemoved(Instance* _this, Instance* child)
{
    if (child && IsA<Render::SpecialMesh>(child))
    {
        RBX::StandardOut::print(RBX::MESSAGE_INFO,
            "Remove old %s", child->name.c_str());
        Render::SpecialMesh* mesh = toInstance<Render::SpecialMesh>(child);
        Render::IRenderable* parent = toInstance<Render::IRenderable>(child->parent);

        if (parent)
        {
            RBX::StandardOut::print(RBX::MESSAGE_INFO,
                "Remove %s from %s", child->name.c_str(), parent->name.c_str());
            mesh->removeFromRenderEnvironment();
            parent->specialShape = 0;
            parent->removeFromRenderEnvironment();
            parent->write();
        }
    }
}


void RBX::PVInstance::setFace(NormalId f, SurfaceType s)
{
    switch (f)
    {
    case NormalId::Top:
    {
        top = s;
        break;
    }
    case NormalId::Bottom:
    {
        bottom = s;
        break;
    }
    case NormalId::Left:
    {
        left = s;
        break;
    }
    case NormalId::Right:
    {
        right = s;
        break;
    }
    case NormalId::Front:
    {
        front = s;
        break;
    }
    case NormalId::Back:
    {
        back = s;
        break;
    }
    }
}

float RBX::getAffectedFormFactor(RBX::PVInstance* pv)
{
    switch (pv->getFormFactor())
    {
    case RBX::Brick:
    {
        return 1.2f;
    }
    case RBX::Plate:
    {
        return 0.4f;
    }
    case RBX::Symmetric:
    {
        return 1.0f;
    }
    }
}

RBX::SurfaceType RBX::PVInstance::getSurface(NormalId face)
{
    switch (face)
    {
    case NormalId::Top:
    {
        return top;
    }
    case NormalId::Bottom:
    {
        return bottom;
    }
    case NormalId::Front:
    {
        return front;
    }
    case NormalId::Back:
    {
        return back;
    }
    case NormalId::Left:
    {
        return left;
    }
    case NormalId::Right:
    {
        return right;
    }
    }
    return Smooth;
}

void RBX::PVInstance::initializeForKernel()
{
    if (!primitive->geom[0])
    {

        primitive->createPrimitive(shape, size);
        primitive->modifyUserdata(this);

    }

    if (primitive->body && !primitive->body->created())
    {
        primitive->body->createBody(size);

        primitive->body->attachPrimitive(primitive);
        primitive->body->modifyUserdata(this);
    }

    setAnchored(getAnchored());
    setCanCollide(getCanCollide());
}

RBX::PVInstance::~PVInstance()
{

}

void RBX::PVInstance::onRemove()
{
    if (primitive)
    {
        primitive->destroyPrimitive();
        if (primitive->body)
        {
            primitive->body->destroyBody();
        }
        Connector* connecting = JointsService::get()->getConnecting(primitive);
        if (connecting)
        {
            connecting->remove();
        }
    }
}

RBX::PVInstance::PVInstance()
{
    size = Vector3(4.f, 1.2f, 2.f);
    color = Color3(0.639216f, 0.635294f, 0.643137f);

    setClassName("PVInstance");
    setName("PVInstance");

    formFactor = FormFactor::Symmetric;

    elasticity = 0.5f;
    friction = 0.1f;

    canCollide = true;
    anchored = false;

    alpha = 1;
    shape = Block;

    primitive = new Primitive(new Body());
    primitive->onPVChanged.connect(onPrimitivePVChanged);

   // onChildAdded.connect(onMeshAdded);
    onChildRemoved.connect(onMeshRemoved);

    Gurnel::get()->addPrimitive(primitive);

    pv = primitive->pv;
    startPV = new PV();

    fauxTransparency = 0;
}