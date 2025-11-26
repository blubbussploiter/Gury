
#include "../Gury/Game/Objects/PVInstance/pvinstance.h"
#include "../Gury/Game/World/runservice.h"
#include "../Gury/Game/World/ray.h"
#include "../Gury/Game/rbxmath.h"

#include "humanoid.h"

using namespace RBX;

RBX::Sound* whoosh = RBX::Sound::fromFile("rbxasset://sounds\\button.wav");
RBX::Sound* bsls_steps = RBX::Sound::fromFile("rbxasset://sounds\\bfsl-minifigfoots1.mp3", 1);

void RBX::Humanoid::setLegCollisions(bool collidable)
{
    if (rightLeg && leftLeg)
    {
        if (rightLeg->primitive && leftLeg->primitive)
        {
            rightLeg->primitive->modifyCollisions(collidable);
            leftLeg->primitive->modifyCollisions(collidable);
        }
    }
}

void RBX::Humanoid::setArmCollisions(bool collidable)
{
    if (rightArm && leftArm)
    {
        if (rightArm->primitive && leftArm->primitive)
        {
            rightArm->primitive->modifyCollisions(collidable);
            leftArm->primitive->modifyCollisions(collidable);
        }
    }
}

void RBX::Humanoid::getHumanoidGroundData(float& distanceFrom, Vector3& hit, Vector3& normal)
{
    CoordinateFrame cframe = humanoidRootPart->getCoordinateFrame();

    Ray ray = Ray::fromOriginAndDirection(cframe.translation, -Vector3::unitY());

    RBX::ISelectable* maybeGround = World::getPartFromG3DRay<Instance>(ray, hit, true, *getParent()->getChildren());

    hit = hit;
    normal = normalize(hit);
    distanceFrom = (ray.origin - hit).magnitude();
}

void Humanoid::adjustLimbCollisions()
{
    setLegCollisions(false);
    setArmCollisions(false);
}

void RBX::Humanoid::tryEnable()
{
    Body* body = humanoidHead->getBody();
    if ((body && body->body) && !dBodyIsEnabled(body->body))
    {
        dBodyEnable(body->body);
    }
}

void RBX::Humanoid::editMass()
{
    Body* body = humanoidHead->getBody();
    if (body && body->body)
    {
        CoordinateFrame cofm = humanoidRootPart->getCoordinateFrame();
        dMass m;

        float dRotation[12] = toDMatrix3(cofm.rotation);

        m = body->getMass();

        m.translate(cofm.translation.x, cofm.translation.y, cofm.translation.z);
        m.rotate(dRotation);
        m.adjust(0.1f);

        body->modifyMass(m);
    }
}

void RBX::Humanoid::applyHipHeight()
{
    Body* body = humanoidHead->getBody();

    if (currentlyJumping) {
        return;
    }

    if (body && body->body)
    {
        float distance;
        Vector3 hit, normal;
        getHumanoidGroundData(distance, hit, normal);

        Vector3 position = humanoidRootPart->getPosition();
        Vector3 velocity = body->pv->velocity.linear;

        Vector3 desired = Vector3(position.x, hit.y + hipHeight, position.z);

        if (body->getFloatMass() > 1.5f)
        {
            editMass();
        }

        if (distance <= hipHeight)
        {
            body->applyForce(Vector3(0, -velocity.y, 0));
            body->modifyPosition(desired);
        }
    }
}

void RBX::Humanoid::getFeetOffGround()
{
    float damper = 0.2f, multiplier = 2.f;

    CoordinateFrame cframe = humanoidHead->getCoordinateFrame();
    Quat rotation = cframe.rotation.inverse();
    Body* body = humanoidHead->getBody();

    if (body && body->body) {

        Vector3 rotVelocity = humanoidHead->getRotVelocity();
        Vector3 torque = ((Vector3(rotation.x, 0, rotation.z) - rotVelocity / 2 * damper) * multiplier) * body->getFloatMass() * 1 / RunService::get()->deltaTime;

        attemptingToBalance = (cframe.rotation == rotation);
        dBodyAddTorque(body->body, torque.x, torque.y, torque.z);

    }
}

void RBX::Humanoid::onTurn()
{
    Body* body = humanoidHead->getBody();

    if (body && body->body)
    {
        CoordinateFrame humanoidCFrame = humanoidRootPart->getCoordinateFrame();
        Vector3 origin = humanoidCFrame.translation;
        Vector3 target = humanoidCFrame.translation + walkDirection;

        Vector3 lookPosition = Vector3(target.x, 0, target.z);
        origin.y = 0;

        float magnitude = 2;

        float angle = Math::angleFromTwoPoints(origin, lookPosition);
        float angRad = toRadians(angle);

    }

}

void RBX::Humanoid::onMovement()
{
    Body* body = humanoidHead->getBody();
    Vector3 velocity = humanoidHead->getVelocity();

    float speed = velocity.length();

    if (speed < 8.56f)
    {
        Vector3 strafeVelocity(walkDirection.x, velocity.y, walkDirection.z);
        humanoidHead->setVelocity(strafeVelocity);
    }

}

void RBX::Humanoid::onJump()
{
    Body* body = humanoidHead->getBody();
    Vector3 velocity = body->pv->velocity.linear;
    if (!body || !body->body) return;

    if (jumping)
    {
        body->applyForceAtRelativePosition(Vector3(0, 7.5f, 0), Vector3(0, -hipHeight / 2, 0));
        setLegCollisions(true);

        whoosh->play();

        jumping = false;
        currentlyJumping = true;
    }

    if (whoosh->isPlaying()
        && jumpClock > 0.05f)
    {
        whoosh->stop();
    }

    switch (humanoidState)
       {
        case Jumping:
        {

            break;
        }
        case Landed:
        {
            setLegCollisions(false);
            currentlyJumping = false;
            break;
        }
        default: break;
    }

}

void RBX::Humanoid::doSounds()
{
    switch (humanoidState)
    {
    case Strafing:
    {
        bsls_steps->playOnce();
        break;
    }
    case Running:
    {
        bsls_steps->stop();
        break;
    }
    }
}

void RBX::Humanoid::onStrafe()
{
    if (humanoidHead)
    {
        Body* body = humanoidHead->getBody();
        Vector3 rotationalVelocity = humanoidHead->getRotVelocity();
        Vector3 linearVelocity = humanoidHead->getVelocity();

        if (body)
        {
            if (body->isValid())
            {

                if (humanoidStanding())
                {
                    tryEnable();
                    onJump();

                    switch (walkMode)
                    {
                    case DIRECTION_MOVE:
                    {
                        humanoidState = Strafing;

                        if (!currentlyJumping)
                        {
                            onTurn();
                        }

                        onMovement();

                        break;
                    }
                    default:
                    {
                        if (walkDirection == Vector3::zero())
                        {
                            if (isGrounded())
                            {
                                if (linearVelocity.length() > 0)
                                {
                                    Vector3 brakeVelocity = -linearVelocity;
                                    Vector3 breakRotVelocity = -rotationalVelocity * 0.25f;
                                    body->applyForce(Vector3(brakeVelocity.x, 0, brakeVelocity.z));
                                    //body->applyTorque(Vector3(breakRotVelocity.x, 0, breakRotVelocity.z));
                                    //body->modifyVelocity(Velocity(body->pv->velocity.linear, Vector3::zero()));
                                }
                            }
                        }
                        break;
                    }
                    }

                    getFeetOffGround();

                }
            }
        }
    }

}