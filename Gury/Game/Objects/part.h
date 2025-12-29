
#pragma once

#include "../Gury/Game/Objects/PVInstance/pvinstance.h"
#include "../Gury/Game/Rendering/brickcolor.h"
#include "../Gury/Game/Reflection/enum.h"
#include "../../Game/Objects/tileBuilder.h"

namespace RBX
{

	namespace Render
	{
		class SpecialMesh;
	}

	class PartInstance :
		public RBX::PVInstance
	{
	protected:
		friend class RBX::TileBuilder;
		friend class RBX::Render::SpecialMesh;

		float sphereRadius; /* 2025 you know what this is for :) */
		float surfaceAlpha;

		/* Mesh things */

		Vector3 getQuadFaceNormal(NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);
		Array<Vector3> getBrickFaceVertices(NormalId face, bool asWorldSpace = true);

		void reorderSurfaces(Color4 oldColor);

		void generateSubdividedFace(Array<Vector3>& out, Array<Vector2>& texCoordsOut, NormalId face);
		void writeBrickGeometry(NormalId face, Array<Vector3> vertices, Array<Vector2> texCoords);

		void writeBrickFace(NormalId face);
		void editMeshPosition();

		void writeBall();

		void regenerateRenderable();
		void regenerateFace(NormalId face);

		static Array<Vector3> calculateBrickFaceVertices(NormalId face, Vector3 size);
		static Vector2 getSubdivisionNumbers(NormalId face, Vector3 size);

	public:

		Vector3 size;
		Color4 color;

		BrickColor brickColor;

		SurfaceType front, back, top, bottom, right, left;

		FormFactor formFactor;

		bool showControllerFlag;
		bool canCollide;
		bool anchored;
		bool locked;

		float fauxTransparency; /* the version set by reflection */

		Shape shape;

		void setShape(Shape s)
		{
			Shape oldShape = shape;

			shape = s;

			switch (shape)
			{
			case Ball:
			{
				size.x = size.y;
				size.z = size.x;
				break;
			}
			}

			if (oldShape != s) /* Remove old shape */
			{
				Render::WorldManager::get()->makeDirty();
			}

			primitive->modifyShape(shape);
		}

		Shape getShape()
		{
			return shape;
		}

		RBX::SurfaceType getFrontSurface() { return front; }
		void setFrontSurface(RBX::SurfaceType s) { setFace(Front, s); }

		RBX::SurfaceType getBackSurface() { return back; }
		void setBackSurface(RBX::SurfaceType s) { setFace(Back, s); }

		RBX::SurfaceType getRightSurface() { return right; }
		void setRightSurface(RBX::SurfaceType s) { setFace(Right, s); }

		RBX::SurfaceType getLeftSurface() { return left; }
		void setLeftSurface(RBX::SurfaceType s) { setFace(Left, s); }

		RBX::SurfaceType getTopSurface() { return top; }
		void setTopSurface(RBX::SurfaceType s) { setFace(Top, s); }

		RBX::SurfaceType getBottomSurface() { return bottom; }
		void setBottomSurface(RBX::SurfaceType s) { setFace(Bottom, s); }

		void setBrickColor(int bc);

		int getBrickColor()
		{
			return brickColor.number.number;
		}

		void setAnchored(bool a);

		void setCanCollide(bool c)
		{
			canCollide = c;
			getPrimitive()->modifyCollisions(canCollide);
		}

		FormFactor getFormFactor() { return formFactor; }

		void setFormFactor(FormFactor f)
		{
			formFactor = f;
			edit();
			onChanged(this, getPropertyByName("formFactor"));
		}

		bool getShowControllerFlag() { return showControllerFlag; }
		void setShowControllerFlag(bool scf) { showControllerFlag = scf; }

		void write();
		void edit();
		void setFace(NormalId f, SurfaceType s);

		void doWrite(bool orderNewSurfaces);

		Render::SpecialMesh* getSpecialMesh();

		void removeSurfaces(Color4 surfaceColor);
		void orderSurfaces(Color4 surfaceColor);
		void orderSurface(Color4 surfaceColor, SurfaceType surfaceType);
		void removeSurface(Color4 surfaceColor, SurfaceType surfaceType);

		bool containSurface(SurfaceType surface);

		Vector3 getSizeExternal()
		{
			return getSize() * 2;
		}

		Vector3 getSize()
		{
			return size;
		}

		void setSize(Vector3 s) {

			Vector3 oldSize = size;

			size.x = round(s.x);
			size.y = s.y; /* do form factor here */
			size.z = round(s.z);
			size /= 2;

			switch (shape)
			{
			case Ball:
			{
				float xyz = std::max(std::max(size.x, size.y), size.z);
				size = Vector3(xyz, xyz, xyz);
				break;
			}
			case Cylinder: break;
			}

			if (primitive)
			{
				primitive->modifySize(size);
			}
			if (primitive && primitive->body)
			{
				primitive->body->modifySize(size);
			}

			if (inRenderEnvironment())
			{
				if (oldSize != size)
				{
					Render::WorldManager::get()->makeDirty();
				}
			}

			onChanged(this, getPropertyByName("Size"));
		}

		Color4 getColor() { return color; }

		void setColor(Color3 c) {

			Color4 _color;

			_color = c;
			_color.a = alpha;

			setColor4(_color);
		}

		void setColor4(Color4 newColor);

		bool getAnchored() { return anchored; }
		bool getCanCollide() { return canCollide; }
		bool getLocked() { return locked; }

		SurfaceType getSurface(NormalId face);

		float getFauxTransparency()
		{
			return fauxTransparency;
		}

		float getTransparency()
		{
			return transparency;
		}
		void setTransparency(float f)
		{
			float oldTransparency = transparency;

			transparency = f;

			alpha = 1 - transparency;
			fauxTransparency = f;

			if (oldTransparency != f)
			{
				setColor4(Color4(color.r, color.g, color.b, alpha));
			}

		}

		float getReflectance()
		{
			return reflectance;
		}

		void setReflectance(float r)
		{
			reflectance = r;
		}

		void setLocked(bool l) { locked = l; }

		Box getBox()
		{
			return getInstanceGeometry().getBox();
		}

		Render::Geometry getBoundingBox()
		{
			return getInstanceGeometry();
		}

		Render::Geometry getInstanceGeometry()
		{
			return Render::Geometry(getCoordinateFrame(), size, shape);
		}

		Extents getLocalExtents() { return Extents(-getSize(), getSize()); }
		Extents getWorldExtents()
		{
			Extents localExtents = getLocalExtents();
			return localExtents.toWorldSpace(getCFrame());
		}

		static NormalId getOppositeNormalId(NormalId n)
		{
			switch (n)
			{
			case Top: return Bottom;
			case Bottom: return Top;
			case Right: return Left;
			case Left: return Right;
			case Front: return Back;
			case Back: return Front;
			default: return UNDEFINED;
			}
		}

		void CreatePhysicalPresence();

		void step();

		~PartInstance();
		PartInstance();

		RBX_CLONE_DEF(PartInstance) 
		RTTR_ENABLE(RBX::PVInstance)
	};

	extern CoordinateFrame getSurfaceCenter(NormalId face, Vector3 size, Extents e);
	extern float getAffectedFormFactor(RBX::PartInstance* pv);
}
