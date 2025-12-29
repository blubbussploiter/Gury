#pragma once

#include "../Gury/Game/Objects/part.h"
#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Services/content.h"
#include "../Gury/Game/Services/stdout.h"

namespace RBX
{
	namespace Render
	{
		enum MeshType
		{
			Head,
			Torso,
			Wedge,
			Sphere,
			Cylinder,
			Filemesh,
			Brick
		};

		class MeshBase : public IRenderable
		{
		public:
			Color4 color;
			Vector3 mesh_scale;
		};

		class HeadMesh
		{
		public:

			static void buildTube(Vector3 from, Vector3 to, int slices, Array<Vector3>& vertices, Array<Vector3>& normals);

			static void buildCircle(float circleRadius, float circleY, int triangles, Array<Vector3>& vertices, Array<Vector3>& normals, bool invertSecondVertex);

			static void getFaceVertices(Vector3 size, NormalId face, Array<Vector3>& vertices, Array<Vector3>& normals, Array<Vector3>& uvs);

		};

		class WedgeMesh
		{
		public:

			static Array<Vector3> getFaceVertices(CoordinateFrame position, Vector3 size, NormalId face);

		};

		class SpecialMesh :
				public MeshBase
			{
			private:
				Content meshId;
				Array<Vector3> vertices;
				Array<Vector3> normals;
				Array<Vector3> uvs;
				MeshType meshType;
				int faces;
				int num_faces;
			public:

				MeshType getMeshType() { return meshType; }

				void setMeshType(MeshType _meshType)
				{
					if (_meshType != meshType)
					{
						meshType = _meshType;
						removeFromRenderEnvironment();
						write();
					}

				}

				void setMeshScale(Vector3 scale)
				{
					if (mesh_scale != scale)
					{
						mesh_scale = scale;
						resizeMesh();
					}
				}
				
				CoordinateFrame getParentCoordinateFrame() {
					PartInstance* pvInstance = toInstance<PartInstance>(parent);
					if (pvInstance) {
						return pvInstance->getCoordinateFrame();
					}
					return CoordinateFrame();
				}

				Vector3 getParentSize()
				{
					PartInstance* pvInstance = toInstance<PartInstance>(parent);
					if (pvInstance) {
						return pvInstance->getSize();
					}
					return Vector3();
				}

				Vector3 getMeshScale() { return mesh_scale; }

				void resizeMesh(Vector3 scale);
				void resizeMesh();

				void fromFile(std::string path);
				void fromMeshType(MeshType types);

				void setMeshId(Content meshId);
				Content getMeshId() { return meshId; }

				void write();

				void edit();

				void writeFace(NormalId Face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);
				void writeTriangleFace(NormalId Face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2);

				void editFace(NormalId Face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);
				void editTriangleFace(NormalId Face, Vector2 xy, Vector2 wh, Vector3 v0, Vector3 v1, Vector3 v2);

				void writeSpecialMesh();
				void editSpecialMesh();

				void writeWedgeFace(NormalId face);
				void editWedgeFace(NormalId face);

				void writeWedge();
				void editWedge();

				void writeHead();
				void editMeshPosition();

				void removeFromRenderEnvironment();

				static void onParentChanged(Instance* self, rttr::property propertyName);
				static void onParentSizeChanged(Instance* self, rttr::property propertyName);

				SpecialMesh()
				{
					setClassName("SpecialMesh");
					setName("SpecialMesh");
					setMeshType(Head);

					onChanged.connect(onParentChanged);

					mesh_scale = Vector3::one();
				}
				virtual ~SpecialMesh() {}

				RBX_CLONE_DEF(SpecialMesh)
					RTTR_ENABLE(RBX::Render::IRenderable)
			};

	}
}
