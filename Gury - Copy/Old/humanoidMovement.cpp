
#include "../Gury/Game/Objects/instance.h"

#include "../Gury/Game/World/ray.h"
#include "../Gury/Game/rbxmath.h"

#include "humanoid.h"

using namespace RBX;

static Sound* whoosh = Sound::fromFile(GetFileInPath("/content/sounds/button.wav"));

void Humanoid::onMovement()
{
    Body* body = humanoidHead->getBody();
    Vector3 velocity = humanoidHead->getVelocity();

    float speed = velocity.length();

    if (speed < 8.56f)
    {
        Vector3 strafeVelocity(walkDirection.x, velocity.y, walkDirection.z);
        humanoidHead->setVelocity(strafeVelocity * 0.5f);
    }

}

void Humanoid::onJump()
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
        r_turnVelocity = 0;
        break;
    }
    case Landed:
    {
        currentlyJumping = false;
        setLegCollisions(false);
        break;
    }
    default: break;
    }

}

void Humanoid::getFeetOffGround(float damper, float multiplier)
{
    CoordinateFrame cframe = humanoidHead->getCoordinateFrame();
    Body* body = humanoidHead->getBody();

    if (body && body->body)
    {
        dMatrix3 rotation = toDMatrix3(Matrix3::identity());
        dBodySetRotation(body->body, rotation);
    }


}

void Humanoid::onStrafe()
{
    Body* body = humanoidHead->getBody();
    Vector3 velocity = humanoidHead->getVelocity();

    if (body && body->body)
    {

        if (humanoidStanding())
        {

            adjustLimbCollisions();
            tryEnable();

            applyHipHeight();

            switch (walkMode)
            {
            case DIRECTION_MOVE:
            {
                humanoidState = Strafing;
                onMovement();

                if (!currentlyJumping)
                {
                    onTurn();
                }

                break;
            }
            default:
            {
                if (walkDirection == Vector3::zero())
                {
                    if (isGrounded())
                    {
                        if (velocity.length() > 0)
                        {
                            Vector3 brakeVelocity = -velocity * 0.5f;
                            body->applyForce(Vector3(brakeVelocity.x, 0, brakeVelocity.z));
                            body->modifyVelocity(Velocity(body->pv->velocity.linear, Vector3::zero()));
                        }
                    }
                    r_turnVelocity = 0.0f;
                }
                break;
            }
            }

            if (fabs(r_turnVelocity) > 0.0f)
            {
                body->applyTorque(Vector3::unitY() * (r_turnVelocity * 0.1f));
            }

        }

        getFeetOffGround(0.8f, 50);
    }

}