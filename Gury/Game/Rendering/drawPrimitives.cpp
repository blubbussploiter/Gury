#include "../Gury/Game/Objects/PVInstance/pvinstance.h"

void RBX::Primitives::drawLine(Vector2 pos, RenderDevice* d, Color3 color, float width, float height)
{
    Rect2D rect = Rect2D::xywh(pos.x, pos.y, width, height);
    Draw::rect2D(rect, d, color);
}

void RBX::Primitives::rawCylinderAlongX(Color4 color, float radius, float axis, int slices)
{
    GLUquadric* v2; // esi
    GLUquadric* v3; // esi
    GLUquadric* v4; // esi
    GLfloat z; // [esp+98h] [ebp-4h]

    glPushMatrix();
    glColor(color.r, color.g, color.b, color.a);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    z = -axis * 0.5;
    glTranslatef(0.0, 0.0, z);
    v2 = gluNewQuadric();
    gluQuadricDrawStyle(v2, 0x186ACu);
    gluCylinder(v2, radius, radius, axis, slices, 1);
    gluDeleteQuadric(v2);
    glTranslatef(0.0, 0.0, axis);
    v3 = gluNewQuadric();
    gluQuadricDrawStyle(v3, 0x186ACu);
    gluDisk(v3, 0.0, radius, slices, 1);
    gluDeleteQuadric(v3);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, axis);
    v4 = gluNewQuadric();
    gluQuadricDrawStyle(v4, 0x186ACu);
    gluDisk(v4, 0.0, radius, slices, 1);
    gluDeleteQuadric(v4);
    glPopMatrix();
}

void RBX::Primitives::drawBall(RenderDevice* d, RBX::PVInstance* base)
{
    if (base)
    {
        GLUquadric* v1; // esi
        float radius = base->getSize().y;

        glPushMatrix();
        v1 = gluNewQuadric();
        gluQuadricDrawStyle(v1, 0x186ACu);
        gluSphere(v1, radius, 15, 15);
        gluDeleteQuadric(v1);
        glPopMatrix();
    }
}

void RBX::Primitives::drawCylinder(RenderDevice* d, RBX::PVInstance* base)
{
    if (base)
    {
        rawCylinderAlongX(base->color, base->getSize().x, base->getSize().y);
    }
}
