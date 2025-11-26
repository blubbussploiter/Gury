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
         .property("Position", &RBX::PVInstance::getPosition, &RBX::PVInstance::setPosition)(rttr::metadata("Type", RBX::Data), rttr::metadata("Nonserializable", false))
         .property("Velocity", &RBX::PVInstance::getVelocity, &RBX::PVInstance::setVelocity)(rttr::metadata("Type", RBX::Data))
         .property("RotVelocity", &RBX::PVInstance::getRotVelocity, &RBX::PVInstance::setRotVelocity)(rttr::metadata("Type", RBX::Data))
         .property("CFrame", &RBX::PVInstance::getCFrame, &RBX::PVInstance::setCFrame)
         .property("CoordinateFrame", &RBX::PVInstance::getCFrame, &RBX::PVInstance::setCFrame)(rttr::metadata("Nonserializable", false))
         .property("Elasticity", &RBX::PVInstance::getElasticity, &RBX::PVInstance::setElasticity)(rttr::metadata("Type", RBX::Part))
         .property("Friction", &RBX::PVInstance::getFriction, &RBX::PVInstance::setFriction)(rttr::metadata("Type", RBX::Part));
}

void RBX::PVInstance::RemovePhysicalPresence()
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

RBX::PVInstance::~PVInstance()
{
}

RBX::PVInstance::PVInstance()
{
    setClassName("PVInstance");
    setName("PVInstance");

    elasticity = 0.5f;
    friction = 0.1f;

    primitive = new Primitive(new Body());
    pv = primitive->body->pv;
    startPV = new PV();

}