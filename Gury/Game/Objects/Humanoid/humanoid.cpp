
#include "../Gury/Game/rbxmath.h"
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Network/Player/players.h"
#include "../Gury/Game/World/sounds.h"
#include "../Gury/Game/World/ray.h"

#include "../Gury/Kernel/jointsservice.h"
#include "../Gury/Kernel/Connectors/snap.h"

#include "../Gury/Game/Gui/GuiRoot.h"

#include "humanoid.h"

RBX::Sound* uuhhh = RBX::Sound::fromFile("rbxasset://sounds\\uuhhh.wav");
 
RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Humanoid>("Humanoid")
        .constructor<>()
        .property("Health", &RBX::Humanoid::health)(rttr::metadata("Type", RBX::Game))
        .property("health", &RBX::Humanoid::health)(rttr::metadata("Type", RBX::Game));
}

RBX::Humanoid* RBX::Humanoid::modelIsCharacter(RBX::Instance* testModel)
{
    if (testModel)
        return testModel->findFirstChildOfClass<RBX::Humanoid>("Humanoid");
    else
        return 0;
}

void RBX::Humanoid::setLocalTransparency(float transparency)
{
    Instance* parent = getParent();

    for (unsigned int i = 0; i < parent->children->size(); i++)
    {
        Instance* child = parent->children->at(i);
        if (IsA<PVInstance>(child))
        {
            PVInstance* pv = toInstance<PVInstance>(child);
            float f;

            f = pv->fauxTransparency;
            pv->setTransparency(transparency);
            pv->fauxTransparency = f;
        }
    }
}

void RBX::Humanoid::setHumanoidAttributes()
{
    if (!getParent())
        return;

    humanoidHead = getParent()->findFirstChild<PVInstance>("Head");
    humanoidRootPart = getParent()->findFirstChild<PVInstance>("Torso");

}

bool RBX::Humanoid::humanCheck()
{
    if (!humanoidHead || !humanoidRootPart)
    {
        setHumanoidAttributes();
        return 0;
    }
    return 1;
}

bool RBX::Humanoid::isFalling()
{
    Body* body = humanoidRootPart->getBody();

    if (body && body->body)
    {
        if (humanCheck())
        {
            return body->pv->velocity.linear.y >= -0.1f;
        }
    }
    return 0;
}

bool RBX::Humanoid::isJoined()
{
    if (!humanCheck()) return 0;
    return JointsService::areConnectedPrimitives(humanoidRootPart->primitive, humanoidHead->primitive); /* change this probably.. fixi t!!!*/
}

bool RBX::Humanoid::isTripped()
{
    if (humanCheck())
    {
        Vector3 up = humanoidRootPart->getCoordinateFrame().upVector();
        Vector3 absNormUp = normalize(abs(up));
        return (round(absNormUp.y) != 1);
    }
    return 0;
}

bool RBX::Humanoid::isGrounded()
{
    return (getHumanoidGroundData()->distanceFrom < hipHeight);
}

bool RBX::Humanoid::humanoidStanding()
{
    bool standing = true;
    if (humanCheck())
    {
        CoordinateFrame cframe = humanoidRootPart->getCoordinateFrame();
        Body* body = humanoidRootPart->getBody();

        if (isGrounded())
        {
            standing = true;
        }

        if (isTripped())
        {
            standing = false;
        }

        if (isFalling())
        {
            standing = false;
        }
    }
    return standing;
}

bool RBX::Humanoid::limbsCheck()
{
    return humanCheck();
}

void RBX::Humanoid::setWalkDirection(Vector3 value)
{
    double x, z;
    double v4;

    if (value != walkDirection)
    {
        if (value == G3D::Vector3::zero())
        {
            walkDirection = value;
            walkMode = DIRECTION_STAY;
        }
        else
        {
            x = value.x;
            z = value.z;

            v4 = 1.0 / sqrt(z * z + x * x);
            walkDirection.x = x * v4;
            walkDirection.y = 0;
            walkDirection.z = z * v4;
            walkDirection *= walkSpeed; /* walk speed */
            walkMode = DIRECTION_MOVE;
        }
    }
}

void RBX::Humanoid::snap(PVInstance* p0, PVInstance* p1)
{
    if (!p0 || !p1) return;
    SnapConnector* snap = new SnapConnector(p0->primitive, p1->primitive, UNDEFINED);
    snap->build();
}

void RBX::Humanoid::onDied()
{
    if (isDead)
        return;

    isDead = 1;
    uuhhh->play();
}

void RBX::Humanoid::setJump(bool jump)
{
    if (canJump && jump == true)
    {
        jumping = true;
        canJump = false;
    }
    else
        jumping = false;
}

void RBX::Humanoid::jumpTimeStep()
{
    if (!canJump && jumpClock >= jumpTimer)
    {
        canJump = 1;
        jumpClock = 0;
    }
    else
    {
        if (!canJump)
            jumpClock += RunService::get()->deltaTime;
    }
}

void RBX::Humanoid::resetJumpTimer()
{
    canJump = 0;
}

void RBX::Humanoid::buildJoints()
{
    if (!isJoined())
    {
        snap(humanoidRootPart, humanoidHead);
        snap(humanoidRootPart, getRightArm());
        snap(humanoidRootPart, getLeftArm());
        snap(humanoidRootPart, getRightLeg());
        snap(humanoidRootPart, getLeftLeg());

        setLegCollisions(false);
    }
}

void RBX::Humanoid::onKernelStep()
{

    if (!humanCheck())
        return;

    onStrafe();
}

void RBX::Humanoid::onStep()
{

    /* first things first.. build the guy! */

    if (!humanCheck())
        return;

    buildJoints();

    if (!health)
        onDied();

    updateHumanoidState();
    jumpTimeStep();

}

void RBX::Humanoid::updateHumanoidState()
{
    bool fallingOrJumping;
    fallingOrJumping = (humanoidState == Falling || humanoidState == Jumping);

    if (isGrounded() &&
        !fallingOrJumping &&
        walkDirection == Vector3::zero())
    {
        humanoidState = Running;
    }

    if (!isGrounded() && isFalling())
    {
            if (humanoidState != Jumping)
            {
                 humanoidState = Falling;
            }
    }

    if (isGrounded() && fallingOrJumping)
    {
        humanoidState = Landed;
    }

    if (!isGrounded() && currentlyJumping)
    {
        humanoidState = Jumping;
    }

}

void RBX::Humanoid::renderAdornee(RenderDevice* renderDevice)
{
    renderMultiplayer(renderDevice);
}

void RBX::Humanoid::renderMultiplayer(RenderDevice* renderDevice)
{
    GFontRef fnt;
    RBX::Network::Player* localPlayer;

    fnt = Gui::get()->font;
    localPlayer = RBX::Network::Players::get()->localPlayer;

    if (humanCheck())
    {

        if (localPlayer && localPlayer->character == getParent())
        {
            return;
        }

        if (!fnt.isNull())
            /*
            * thanks
            https://github.com/Vulpovile/Blocks3D/blob/a75f7bf381c6714b8c55bb2e72acfb9077c29a08/src/source/DataModelV2/PartInstance.cpp#L114
            */
        {
            Vector3 position = humanoidHead->getPosition();
            Vector3 gamepoint = position + Vector3(0, 1.5, 0);
            Vector3 camerapoint = renderDevice->getCameraToWorldMatrix().translation;
            float distance = pow(pow((double)gamepoint.x - (double)camerapoint.x, 2) + pow((double)gamepoint.y - (double)camerapoint.y, 2) + pow((double)gamepoint.z - (double)camerapoint.z, 2), 0.5);
            if (distance < 100 && distance > -100)
            {
                if (distance < 0)
                    distance = distance * -1;

                renderDevice->pushState();

                    renderDevice->setDepthWrite(0);
                    renderDevice->setDepthTest((RenderDevice::DepthTest)(RenderDevice::DEPTH_ALWAYS_PASS));

                    CoordinateFrame pos(renderDevice->getCameraToWorldMatrix().rotation, gamepoint);

                    fnt->draw3D(renderDevice, getParent()->name, pos, 0.03 * distance, Color3::white(), Color3::black(), G3D::GFont::XALIGN_CENTER, G3D::GFont::YALIGN_CENTER);
                    drawHealthbar(renderDevice, pos, distance);

                    renderDevice->setDepthWrite(1);

                renderDevice->popState();

            }
        }
    }
}

void RBX::Humanoid::drawHealthbar(RenderDevice* rd, CoordinateFrame center, float distance)
{
    float f0 = (distance * 4);

    float width = f0 * (health / maxHealth) * 0.032f;
    float height = distance * 0.015f;

    float x = (distance) * -0.065f;
    float y = (distance) * -0.04f;

    Rect2D green, red;

    rd->setObjectToWorldMatrix(center);

    green = Rect2D::xywh(Vector2(x, y), Vector2(width, height));
    red = Rect2D::xywh(Vector2(x, y), Vector2(f0 * 0.032f, height));

    Draw::rect2D(red, rd, Color3::red());
    Draw::rect2D(green, rd, Color3(0.50588238f, 0.77254903f, 0.086274512f));

}

RBX::PVInstance* RBX::Humanoid::getRightArm()
{
    if (!rightArm)
    {
        rightArm = getParent()->findFirstChild<PVInstance>("Right Arm");
    }
    return rightArm;
}

RBX::PVInstance* RBX::Humanoid::getLeftArm()
{
    if (!leftArm)
    {
        leftArm = getParent()->findFirstChild<PVInstance>("Left Arm");
    }
    return leftArm;
}

RBX::PVInstance* RBX::Humanoid::getRightLeg()
{
    if (!rightLeg)
    {
        rightLeg = getParent()->findFirstChild<PVInstance>("Right Leg");
    }
    return rightLeg;
}

RBX::PVInstance* RBX::Humanoid::getLeftLeg()
{
    if (!leftLeg)
    {
        leftLeg = getParent()->findFirstChild<PVInstance>("Left Leg");
    }
    return leftLeg;
}
