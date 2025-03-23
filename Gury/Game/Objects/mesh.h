
#include "../Gury/Game/Objects/PVInstance/pvinstance.h"
#include "../Gury/Game/Objects/PVInstance/pvenums.h"
#include "../Gury/Game/Services/content.h"
#include "../Gury/Game/Services/stdout.h"

namespace RBX
{
	namespace Render
	{
		extern void buildHeadMesh(Vector3 size);

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

		class WedgeMesh : public MeshBase
		{
		public:

			Array<Vector3> getFaceVertices(CoordinateFrame position, Vector3 size, NormalId face);

			void writeFace(NormalId Face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);
			void writeTriangleFace(NormalId Face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2);

			void editFace(NormalId Face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 v5);
			void editTriangleFace(NormalId Face, Vector2 uv, Vector3 v0, Vector3 v1, Vector3 v2);

			void writeWedgeFace(NormalId face);
			void editWedgeFace(NormalId face);

			void writeWedge();
			void editWedge();

		};

		class SpecialMesh : 
			public WedgeMesh
		{
		private:
			Content meshId;
			std::vector<Vector3> vertices;
			std::vector<Vector3> normals;
			std::vector<Vector3> uvs;
			MeshType meshType;
			int faces;
			int num_faces;
		public:

			MeshType getMeshType() { return meshType; }

			void setMeshType(MeshType _meshType)
			{
				switch (_meshType)
				{
					case Head:
					{
						fromFile(GetFileInPath("/content/font/head.mesh"));
						break;
					}
					default: break;
				}

				if (_meshType != meshType)
				{
					meshType = _meshType;
					removeFromRenderEnvironment();
					write();
				}

			}

			void setMeshScale(Vector3 scale)
			{
				RBX::PVInstance* parent = dynamic_cast<RBX::PVInstance*>(getParent());
				if (parent)
				{
					Vector3 sz = parent->getSize();

					mesh_scale = scale * 1.75f;

					if (sz.y > mesh_scale.y && sz.x > mesh_scale.x)
					{
						mesh_scale *= sz;
					}
				}
				edit();
			}

			Vector3 getMeshScale() { return mesh_scale; }

			void fromFile(std::string path);
			void fromMeshType(MeshType types);

			void setMeshId(Content meshId);
			Content getMeshId() { return meshId; }
			
			void write();

			void edit();

			void writeSpecialMesh();
			void editSpecialMesh();

			SpecialMesh()
			{
				setClassName("SpecialMesh");
				setName("SpecialMesh");
				setMeshType(Head);
				
				mesh_scale = Vector3::one();
			}
			virtual ~SpecialMesh() {}

			RTTR_ENABLE(RBX::Render::IRenderable)
		};

	}
}
