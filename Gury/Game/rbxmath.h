#pragma once

#include <G3DAll.h>
#include "../Gury/Game/Objects/PVInstance/pvenums.h"

namespace RBX
{
    namespace Math
    {

        static Vector3 getEulerAngles(Matrix3 rotation)
        {
            Vector3 euler;
            rotation.toEulerAnglesXYZ(euler.x, euler.y, euler.z);
            return euler;
        }

        static float angleFromTwoPoints(Vector3 from, Vector3 to)
        {
            float x1 = from.x, x2 = to.x, z1 = from.z, z2 = to.z;
            float dist = sqrt(x2 - x1 * x2 - x1 + z2 - z1 * z2 - x1);
            return dist;
        }

        static Vector3 rotationFromTwoPoints(Vector3 from, Vector3 to)
        {
            return Vector3(acos(dot(from.yz(), to.yz())),
                acos(dot(from.xz(), to.xz())),
                acos(dot(from.xy(), to.xy())));
        }

        /* i ripped all of these from IDA */
        static float angleToE0(const G3D::Vector3 v)
        {
            float result; // st7
            G3D::Vector3 u; // [esp+0h] [ebp-Ch] BYREF
            float y; // [esp+8h] [ebp-4h]

            u.y = v.x;
            y = v.z;

            u.unitize(0.000001f);
            result = acos(u.y);

            if (y < 0.0f)
                return 6.2831855f - result;

            return result;
        }
        static float radWrap(float rad)
        {
            float result; // st7
            float v2; // [esp+Ch] [ebp-4h]

            result = rad;
            if (rad < -3.1415927f || result >= 3.1415927f)
            {
                v2 = floor((result + 3.1415927f) * 0.15915495f);
                return rad - (float)v2 * 6.283185f;
            }
            return result;
        }
        static RBX::NormalId getClosestObjectNormalId(const G3D::Vector3* worldV, const G3D::Matrix3* objectR)
        {
            long double v2; // st7
            long double v3; // st6
            long double v4; // st5
            long double v5; // st3
            long double v6; // st2
            long double v7; // st4
            RBX::NormalId result; // eax
            long double v9; // st7

            v2 = objectR->elt[1][0] * worldV->y + objectR->elt[2][0] * worldV->z + worldV->x * objectR->elt[0][0];
            v3 = objectR->elt[1][1] * worldV->y + objectR->elt[0][1] * worldV->x + objectR->elt[2][1] * worldV->z;
            v4 = objectR->elt[1][2] * worldV->y + objectR->elt[0][2] * worldV->x + objectR->elt[2][2] * worldV->z;
            v5 = fabs(v3);
            v6 = fabs(v2);
            v7 = fabs(v4);
            result = RBX::NormalId::UNDEFINED;
            if (v6 > v5)
            {
                if (v7 < v6)
                {
                    if (v2 <= 0.0)
                        return RBX::Left;
                    else
                        return RBX::Right;
                }
                v9 = v4;
                goto LABEL_10;
            }
            v9 = v4;
            if (v7 >= v5)
            {
            LABEL_10:
                result = RBX::Back;
                if (v9 <= 0.0)
                    return RBX::Front;
                return result;
            }
            result = RBX::Top;
            if (v3 <= 0.0)
                return RBX::Bottom;
            return result;
        }
        static Vector3 toDiagonal(Matrix3 m)
        {
            return Vector3(m.elt[0][1], m.elt[1][1], m.elt[2][2]);
        }
    }
}
