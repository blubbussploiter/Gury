
#include "part.h"
#include "mesh.h"

#include "../Gury/Game/World/lighting.h"
#include "../Gury/Game/Services/stdout.h"
#include "../Gury/Kernel/jointsservice.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<RBX::PartInstance>("Part")
		 .constructor<>()
		 .property("Anchored", &RBX::PartInstance::getAnchored, &RBX::PartInstance::setAnchored)(rttr::metadata("Type", RBX::Behavior))
		 .property("CanCollide", &RBX::PartInstance::getCanCollide, &RBX::PartInstance::setCanCollide)(rttr::metadata("Type", RBX::Behavior))
		 .property("Locked", &RBX::PartInstance::getLocked,
			 &RBX::PartInstance::setLocked)(rttr::metadata("Type", RBX::Behavior))
		 .property("Color", &RBX::PartInstance::getColor, &RBX::PartInstance::setColor4)(rttr::metadata("Nonserializable", false))
         .property("Shape", &RBX::PartInstance::getShape, &RBX::PartInstance::setShape)(rttr::metadata("Type", RBX::Part))
         .property("shape", &RBX::PartInstance::getShape, &RBX::PartInstance::setShape)(rttr::metadata("Nonserializable", false))
         .property("size", &RBX::PartInstance::getSizeExternal, &RBX::PartInstance::setSize)(rttr::metadata("Type", RBX::Part))
         .property("Size", &RBX::PartInstance::getSizeExternal, &RBX::PartInstance::setSize)(rttr::metadata("Nonserializable", false))
         .property("FrontSurface", &RBX::PartInstance::getFrontSurface, &RBX::PartInstance::setFrontSurface)(rttr::metadata("Type", RBX::Surface))
         .property("BackSurface", &RBX::PartInstance::getBackSurface, &RBX::PartInstance::setBackSurface)(rttr::metadata("Type", RBX::Surface))
         .property("TopSurface", &RBX::PartInstance::getTopSurface, &RBX::PartInstance::setTopSurface)(rttr::metadata("Type", RBX::Surface))
         .property("BottomSurface", &RBX::PartInstance::getBottomSurface, &RBX::PartInstance::setBottomSurface)(rttr::metadata("Type", RBX::Surface))
         .property("RightSurface", &RBX::PartInstance::getRightSurface, &RBX::PartInstance::setRightSurface)(rttr::metadata("Type", RBX::Surface))
         .property("LeftSurface", &RBX::PartInstance::getLeftSurface, &RBX::PartInstance::setLeftSurface)(rttr::metadata("Type", RBX::Surface))
         .property("rawFormFactor", &RBX::PartInstance::getFormFactor, &RBX::PartInstance::setFormFactor)(rttr::metadata("Nonserializable", false))
         .property("formFactor", &RBX::PartInstance::getFormFactor, &RBX::PartInstance::setFormFactor)(rttr::metadata("Nonserializable", false))
         .property("FormFactor", &RBX::PartInstance::getFormFactor, &RBX::PartInstance::setFormFactor)
         .property("Transparency", &RBX::PartInstance::getFauxTransparency, &RBX::PartInstance::setTransparency)(rttr::metadata("Type", RBX::Appearance))
		 .property("BrickColor", &RBX::PartInstance::getBrickColor, &RBX::PartInstance::setBrickColor);
}

void RBX::PartInstance::setBrickColor(int bc)
{
	brickColor = BrickColor::getBrickMap()->brickColorFromNumber(bc);
	setColor(brickColor.color);
}

void RBX::PartInstance::setAnchored(bool a)
{
    anchored = a;
    if (primitive->body)
    {
        if (anchored)
        {
            primitive->body->detachPrimitive(primitive);
            primitive->body->destroyBody();
            primitive->body = 0;
        }
    }
    else
    {
        if (!anchored)
        {
            primitive->body = new Body();
            primitive->body->modifyPosition(pv->position);
            primitive->body->createBody(size);
            primitive->body->attachPrimitive(primitive);
        }
    }
    onChanged(this, getPropertyByName("Anchored"));
}

void RBX::PartInstance::write()
{
    doWrite(true);
}

void RBX::PartInstance::doWrite(bool orderNewSurfaces)
{
    if (specialShape)
    {
        specialShape->write();
    }
    else
    {
        if (orderNewSurfaces)
        {
            orderSurfaces(color);
        }

        switch (shape)
        {
        case Cylinder:
        case Block:
        {
            /* in order 0 - 5*/

            writeBrickFace(Right);
            writeBrickFace(Top);
            writeBrickFace(Back);
            writeBrickFace(Left);
            writeBrickFace(Bottom);
            writeBrickFace(Front);

            break;
        }
        case Ball:
        {
            sphereRadius = size.y;
            writeBall();
            break;
        }
        }

    }
}

RBX::Render::SpecialMesh* RBX::PartInstance::getSpecialMesh()
{
    return findFirstChildOfClass<Render::SpecialMesh>("SpecialMesh");
}

void RBX::PartInstance::edit()
{
    Render::SpecialMesh* mesh = getSpecialMesh();
    if (mesh)
    {
        specialShape->edit();
    }
    else
    {
        editMeshPosition();
        editGlobalProxyLocation();
    }
}

void RBX::PartInstance::setFace(NormalId f, SurfaceType s)
{
    SurfaceType oldSurfaceType = Smooth;

    switch (f)
    {
    case NormalId::Top:
    {
        oldSurfaceType = top;
        top = s;
        break;
    }
    case NormalId::Bottom:
    {
        oldSurfaceType = bottom;
        bottom = s;
        break;
    }
    case NormalId::Left:
    {
        oldSurfaceType = left;
        left = s;
        break;
    }
    case NormalId::Right:
    {
        oldSurfaceType = right;
        right = s;
        break;
    }
    case NormalId::Front:
    {
        oldSurfaceType = front;
        front = s;
        break;
    }
    case NormalId::Back:
    {
        oldSurfaceType = back;
        back = s;
        break;
    }
    }

    if (oldSurfaceType != s)
    {
        Render::WorldManager::get()->makeDirty();
    }
}

void RBX::PartInstance::setColor4(Color4 newColor)
{
    color = newColor;
    Render::WorldManager::get()->makeDirty();
}


float RBX::getAffectedFormFactor(RBX::PartInstance* pv)
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

RBX::SurfaceType RBX::PartInstance::getSurface(NormalId face)
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

void RBX::PartInstance::step()
{
    Render::SpecialMesh* mesh = getSpecialMesh();
    if (mesh)
    {
        mesh->edit();
    }
    else
    {
        editMeshPosition();
    }
}

void RBX::PartInstance::CreatePhysicalPresence()
{
    if (inRenderEnvironment())
    {
        if (primitive && !primitive->isValid())
        {
            primitive->createPrimitive(shape, size);
            primitive->modifyUserdata(this);

            if (!JointsService::get()->inStaticNode(primitive))
            {
                if (primitive->body)
                {
                    primitive->body->size = size;
                    if (!anchored)
                    {
                        primitive->body->createBody(size);
                        primitive->body->attachPrimitive(primitive);
                    }
                    else
                    {
                        primitive->body->setDisabled(true);
                    }
                    primitive->physicalInstance = this;

                }
            }

            setCanCollide(getCanCollide());

            Gurnel::get()->addPrimitive(primitive);

        }
    }
}

RBX::PartInstance::~PartInstance()
{
    removeFromRenderEnvironment();
    removeSurfaces(color);
}

RBX::PartInstance::PartInstance()
{
    setClassName("Part");
    setName("Part");

    shape = Shape::Block;
    setSize(Vector3(4, 1.2f, 2));

    color = Color3(0.639216f, 0.635294f, 0.643137f);

    formFactor = FormFactor::Symmetric;

    canCollide = true;
    anchored = false;
    locked = false;

    alpha = 1;
    shape = Block;

    fauxTransparency = 0;
    surfaceAlpha = 0;

}
