#pragma once

#include <G3DAll.h>
#include <vector>

#include "../Gury/Kernel/kernel.h"
#include "../Gury/Kernel/primitive.h"

#include "../Gury/Game/Objects/decal.h"
#include "../Gury/Game/Rendering/brickcolor.h"

#include "../Gury/Game/World/extents.h"

#include "../Gury/Game/Objects/IRenderable.h"
#include "../Gury/Game/Objects/ISelectable.h"

#include "../Gury/Game/Rendering/surfaceFactory.h"
#include "../Gury/Game/Rendering/textureReserve.h"

#include "../Gury/Game/Services/stdout.h"

#include "../Gury/Game/rbxmath.h"

namespace RBX
{

	class PVInstance;
	class PartInstance;

	extern CoordinateFrame getSurfaceCenter(NormalId face, Vector3 size, Extents e);
	extern float getAffectedFormFactor(RBX::PVInstance* pv);

	class PVInstance :
		public Render::IRenderable, public RBX::ISelectable
	{
		friend class RBX::PartInstance;
	private:

		float elasticity, friction, erp, cfm;
		float sphereRadius; /* 2025 you know what this is for :) */
		float surfaceAlpha;

		unsigned int idFront;
		unsigned int idBack;
		unsigned int idRight;
		unsigned int idLeft;
		unsigned int idTop;
		unsigned int idBottom;

		unsigned int brickTexture;

		Vector2 cylinderOriginX, cylinderOriginY,
			uv0, uv1, uv2;

		PV* startPV;

		Vector3 getQuadFaceNormal(RBX::NormalId face, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);

		/* Brick Face */

		Array<Vector3> getBrickFaceVertices(NormalId face);

		void writeBrickFaceGeometry(RBX::NormalId face, Vector2 uv, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit = -1); 
		void editBrickFaceGeometry(RBX::NormalId face, Vector2 uv, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5, Color4 color, int textureUnit = -1);

		void writeBrickFace(NormalId face, int unit, bool repeat = false);
		void editBrickFace(NormalId face, int unit, bool repeat = false);
		void editBrickFaces();

		/* Ball face */

		Array<Vector3> getBallFaceVertices();

		void writeBall();
		void editBall();

		Render::TextureReserve::TexturePositionalInformation getFaceUv(NormalId face);

		static void onPrimitivePVChanged(Primitive* primitive);

	public:

		PV* pv;
		Primitive* primitive;

		Vector3 size;
		Color4 color;

		SurfaceType front, back, top, bottom, right, left;

		FormFactor formFactor;

		bool showControllerFlag;
		bool canCollide;
		bool anchored;
		bool locked;

		float fauxTransparency; /* the version set by reflection */

		Shape shape;

	public:

		void resetPV()
		{
			pv->position = startPV->position;
			pv->velocity = startPV->velocity;
			primitive->modifyPosition(pv->position);
			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
		}

		void savePV()
		{
			if (!startPV->position.isIdentity())
			{
				return;
			}
			startPV->position = getCFrame();
			startPV->velocity.linear = getVelocity();
			startPV->velocity.rotational = getRotVelocity();
		}

		void setShape(Shape s)
		{
			Shape oldShape = shape;
			shape = s;

			switch(shape)
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
				removeFromRenderEnvironment();
				write();
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

		Body* getBody()
		{
			if (primitive->body) return primitive->body;
			return 0;
		}

		Primitive* getPrimitive()
		{
			return primitive;
		}

		void setVelocity(Vector3 newVelocity)
		{
			pv->velocity.linear = newVelocity;

			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
		}

		void setRotVelocity(Vector3 newVelocity)
		{
			pv->velocity.rotational = newVelocity;

			if (primitive->body)
			{
				primitive->body->modifyVelocity(pv->velocity);
			}
		}

		void setAnchored(bool a)
		{
			anchored = a;
			if (primitive->body)
			{
				if (anchored)
				{
					primitive->body->detachPrimitive(primitive);
				}
				else
				{
					primitive->body->attachPrimitive(primitive);
				}
			}
		}

		void setCanCollide(bool c)
		{
			canCollide = c;
			getPrimitive()->modifyCollisions(canCollide);
		}

		void setCenter(CoordinateFrame c)
		{
			setCFrame(c);
		}

		FormFactor getFormFactor() { return formFactor; }

		void setFormFactor(FormFactor f)
		{
			formFactor = f;
			edit();
		}

		bool getShowControllerFlag() { return showControllerFlag; }
		void setShowControllerFlag(bool scf) { showControllerFlag = scf; }

		void setFace(NormalId f, SurfaceType s);

		void onRemove();

		void edit();
		void write();

		static void onMeshAdded(Instance* child);
		static void onMeshRemoved(Instance* child);

		void writeSurfaces();
		void editSurfaces();

		void editSurface(NormalId face, unsigned int unit);
		void writeSurface(NormalId face, unsigned int unit);

		Vector3 getSizeExternal()
		{
			return getSize() * 2;
		}

		Vector3 getSize()
		{
			return size;
		}

		void setSize(Vector3 s) {

			size = s;
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
			edit();
		}

		Vector3 getPosition() { return pv->position.translation; }

		void setPosition(Vector3 p)
		{
			setCFrame(CoordinateFrame(pv->position.rotation, p));
		}

		CoordinateFrame getCFrame() {
			if (pv)
			{
				return pv->position;
			}
			return CoordinateFrame();
		}

		void setCFrame(CoordinateFrame cf)
		{
			pv->position = cf;
			primitive->modifyPosition(pv->position);
		}

		Color4 getColor() { return color; }
		void setColor(Color3 c) {
			color = c;  
			color.a = alpha;
			edit();
		}
		void setColor4(Color4 c) {
			color = c;
			edit();
		}
		bool getAnchored() { return anchored; }
		bool getCanCollide() { return canCollide; }
		bool getLocked() { return locked; }

		CoordinateFrame getCoordinateFrame() { return getCFrame(); }

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

			color = Color4(color.r, color.g, color.b, alpha);

			edit();
		}

		float getReflectance()
		{
			return reflectance;
		}

		void setReflectance(float r)
		{
			reflectance = r;
		}

		Vector3 getVelocity() { return pv->velocity.linear; }
		Vector3 getRotVelocity() { return pv->velocity.rotational; }

		Vector3 getEulerAngles()
		{
			return Math::getEulerAngles(pv->position.rotation);
		}

		void lookAt(Vector3 v)
		{
			CoordinateFrame cf;
			cf = getCFrame();
			cf.lookAt(v);
			setCFrame(cf);
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

		CoordinateFrame getCenter()
		{
			return pv->position;
		}

		Extents getLocalExtents() { return Extents(-getSize(), getSize()); }
		Extents getWorldExtents()
		{
			Extents localExtents = getLocalExtents();
			return localExtents.toWorldSpace(getCFrame());
		}

		float getElasticity() { return elasticity; }
		void setElasticity(float el) { elasticity = el; }
		float getFriction() { return friction; }
		void setFriction(float f) { friction = f; }
		float getERP() { return erp; }
		float getCFM() { return cfm; }
		void setCFM(float _cfm) { cfm = _cfm; }
		void setERP(float _erp) { erp = _erp; }
		void initializeForKernel();

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

		~PVInstance();

		PVInstance();

		RTTR_ENABLE(RBX::Render::IRenderable)
	};

	namespace Primitives
	{
		extern void rawCylinderAlongX(Color4 color, float radius, float axis, int slices = 12);
		extern void drawLine(Vector2 pos, RenderDevice* d, Color3 color, float width, float height);
		extern void drawBall(RenderDevice* d, RBX::PVInstance* base);
		extern void drawCylinder(RenderDevice* d, RBX::PVInstance* base);
		static void drawOutline(RenderDevice* rd, Vector3 from, Vector3 to, Color3 outline = Color3::cyan(), float offsetSize = 0.1F)
		{

			Draw::box(Box(Vector3(from.x - offsetSize, from.y + offsetSize, from.z + offsetSize), Vector3(to.x + offsetSize, from.y - offsetSize, from.z - offsetSize)), rd, outline, Color4::clear());
			Draw::box(Box(Vector3(from.x - offsetSize, to.y + offsetSize, from.z + offsetSize), Vector3(to.x + offsetSize, to.y - offsetSize, from.z - offsetSize)), rd, outline, Color4::clear());
			Draw::box(Box(Vector3(from.x - offsetSize, to.y + offsetSize, to.z + offsetSize), Vector3(to.x + offsetSize, to.y - offsetSize, to.z - offsetSize)), rd, outline, Color4::clear());
			Draw::box(Box(Vector3(from.x - offsetSize, from.y + offsetSize, to.z + offsetSize), Vector3(to.x + offsetSize, from.y - offsetSize, to.z - offsetSize)), rd, outline, Color4::clear());

			Draw::box((Box(Vector3(from.x + offsetSize, from.y - offsetSize + 0.1, from.z + offsetSize), Vector3(from.x - offsetSize, to.y + offsetSize - 0.1, from.z - offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(to.x + offsetSize, from.y - offsetSize + 0.1, from.z + offsetSize), Vector3(to.x - offsetSize, to.y + offsetSize - 0.1, from.z - offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(to.x + offsetSize, from.y - offsetSize + 0.1, to.z + offsetSize), Vector3(to.x - offsetSize, to.y + offsetSize - 0.1, to.z - offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(from.x + offsetSize, from.y - offsetSize + 0.1, to.z + offsetSize), Vector3(from.x - offsetSize, to.y + offsetSize - 0.1, to.z - offsetSize))), rd, outline, Color4::clear());

			Draw::box((Box(Vector3(from.x + offsetSize, from.y + offsetSize, from.z - offsetSize), Vector3(from.x - offsetSize, from.y - offsetSize, to.z + offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(from.x + offsetSize, to.y + offsetSize, from.z - offsetSize), Vector3(from.x - offsetSize, to.y - offsetSize, to.z + offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(to.x + offsetSize, from.y + offsetSize, from.z - offsetSize), Vector3(to.x - offsetSize, from.y - offsetSize, to.z + offsetSize))), rd, outline, Color4::clear());
			Draw::box((Box(Vector3(to.x + offsetSize, to.y + offsetSize, from.z - offsetSize), Vector3(to.x - offsetSize, to.y - offsetSize, to.z + offsetSize))), rd, outline, Color4::clear());

		}
	}
}