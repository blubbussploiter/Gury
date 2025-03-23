
#include <G3DAll.h>

#include "../Gury/Game/Objects/PVInstance/pvinstance.h"

Vector3 RBX::getNormalIdDirection(RBX::NormalId normal)
{
	switch (normal)
	{
	case RBX::NormalId::Top:
	{
		return Vector3::unitY();
	}
	case RBX::NormalId::Bottom:
	{
		return -Vector3::unitY();
	}
	case RBX::NormalId::Right:
	{
		return Vector3::unitX();
	}
	case RBX::NormalId::Left:
	{
		return -Vector3::unitX();
	}
	case RBX::NormalId::Front:
	{
		return Vector3::unitZ();
	}
	case RBX::NormalId::Back:
	{
		return -Vector3::unitZ();
	}
	}
	return Vector3::zero();
}

Vector3 RBX::getNormalFromNormalId(RBX::PVInstance* object, RBX::NormalId normal)
{
	return object->getCoordinateFrame().vectorToWorldSpace(getNormalIdDirection(normal));
}