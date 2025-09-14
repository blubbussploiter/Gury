#pragma once

#include <G3DAll.h>

namespace RBX
{
	namespace Render
	{
		class Mesh
		{
		private:

			VARAreaRef worldVisibleGeometry;

			VAR vertexVAR;
			VAR normalVAR;
			VAR texCoordVAR;
			VAR colorVAR;

		public:

			bool varDirty;

			Array<uint32> vertexRefCounts;

			Array<Vector3> originVertexArray; /* Origin of vertices */
			Array<Vector3> vertexArray;
			Array<Vector3> normalArray;
			Array<Vector2> texCoordArray;
			Array<Color4> colorArray;
			Table<uint32, uint32> texCoordUnitsArray;

			class Level
			{
			public:
				Array<uint32> indexArray;
				RenderDevice::Primitive primitive;

				void addIndex(uint32 index);
				void removeIndex(uint32 index);

				Level()
				{
					primitive = RenderDevice::QUADS;
					indexArray = Array<uint32>();
				}

				// I could do : primitive(primitive), but constructor would look too blank
				Level(RenderDevice::Primitive primitive)
				{
					this->primitive = primitive;
					indexArray = Array<uint32>();
				}
			};

			/* unlike roblox, only 1 level per mesh (i lied) */
			Level* triangle_level; 

			Mesh::Level* getLevel()
			{
				return triangle_level;
			}

			int size()
			{
				return vertexRefCounts.size();
			}


			static uint32 write(Vector3 vertex, Vector3 normal, Vector2 texCoord);
			static void edit(uint32 index, Vector3 vertex, Vector3 normal, Vector2 texCoord);
			static void editVertex(uint32 index, Vector3 vertex);
			static void erase(uint32 index);

			static void applyTextureUnits(RenderDevice* renderDevice);

			static void makeVAR();

			static void beginRender(RenderDevice* renderDevice);

			static void sendGeometry(Mesh::Level* level, RenderDevice* renderDevice);

			static void endRender(RenderDevice* renderDevice);

			static void cleanup();

			/* Different mesh per tab, stored in datamodel */
			static Mesh* getGlobalMesh(); 

			Mesh()
			{
				triangle_level = new Level(RenderDevice::TRIANGLES);
			}
		};
	}
}