#pragma once

#include <rttr/registration.h>

namespace G3D { class Vector3; }

namespace RBX
{
	class PVInstance;

	enum NormalId
	{
		Right,
		Top,
		Back,
		Left,
		Bottom,
		Front,
		UNDEFINED
	};

	static RBX::NormalId normals[6] = {
		RBX::NormalId::Front,
		RBX::NormalId::Back,
		RBX::NormalId::Right,
		RBX::NormalId::Left,
		RBX::NormalId::Top,
		RBX::NormalId::Bottom
	};

	extern G3D::Vector3 getNormalIdDirection(RBX::NormalId normal);

	extern G3D::Vector3 getNormalFromNormalId(RBX::PVInstance* object, RBX::NormalId normal);

	enum SurfaceType
	{
		Smooth,
		Glue,
		Weld,
		Studs,
		Inlet,
		UNKNOWN_FILLER,
		Hinge,
		Motor,
		SteppingMotor,
		NumSurfaces,
		UNDEFINED_SURFACE
	};

	enum Shape
	{
		Ball,
		Block,
		Cylinder
	};

	enum FormFactor
	{
		Symmetric,
		Brick,
		Plate
	};

	RTTR_REGISTRATION
	{
		rttr::registration::enumeration<NormalId>("NormalId")
			(
				rttr::value("Right", Right),
				rttr::value("Left", Left),
				rttr::value("Top", Top),
				rttr::value("Bottom", Bottom),
				rttr::value("Front", Front),
				rttr::value("Back", Back)
			);

		rttr::registration::enumeration<SurfaceType>("SurfaceType")
			(
				rttr::value("Smooth", Smooth),
				rttr::value("Glue", Glue),
				rttr::value("Weld", Weld),
				rttr::value("Studs", Studs),
				rttr::value("Inlet", Inlet),
				rttr::value("Hinge", Hinge),
				rttr::value("Motor", Motor),
				rttr::value("SteppingMotor", SteppingMotor)
			);


		rttr::registration::enumeration<Shape>("Shape")
			(
				rttr::value("Ball", Ball),
				rttr::value("Block", Block),
				rttr::value("Cylinder", Cylinder)
			);

		rttr::registration::enumeration<FormFactor>("FormFactor")
			(
				rttr::value("Symmetric", Symmetric),
				rttr::value("Brick", Brick),
				rttr::value("Plate", Plate)
			);
	}
}