

#include "../Gury/Game/Objects/model.h"

#include "renderMesh.h"

RBX::Render::Mesh* mesh;

template<typename T>
void removerHelper(uint32 index, Array<T>& array)
{
	if (array.size() >= (int)index)
	{
		array.remove(index);
	}
}

uint32 RBX::Render::Mesh::write(Vector3 vertex, Vector3 normal, Vector2 texCoord)
{
	Mesh* global = getGlobalMesh(); /* write to a global mesh to keep 'static' */

	if (!global->varDirty)
		global->varDirty = 1;

	uint32 index = global->vertexRefCount++;

	global->vertexRefCounts.append(index);
	global->originVertexArray.append(vertex);
	global->vertexArray.append(vertex);
	global->normalArray.append(normal);

	global->texCoordArray.append(texCoord);

	return index;
}

void RBX::Render::Mesh::edit(uint32 index, Vector3 vertex, Vector3 normal, Vector2 texCoord)
{
	Mesh* global = getGlobalMesh();

	if (!global->varDirty)
		global->varDirty = 1;

	if (global->vertexRefCounts.size() >= index)
	{
		global->vertexArray[index] = vertex;
		global->normalArray[index] = normal;
		global->texCoordArray[index] = texCoord;
	}
	else
	{
		write(vertex, normal, texCoord);
	}

}

void RBX::Render::Mesh::editVertex(uint32 index, Vector3 vertex)
{
	Mesh* global = getGlobalMesh();

	if (!global->varDirty)
		global->varDirty = 1;

	if (global->vertexRefCounts.size() >= index)
	{
		global->vertexArray[index] = vertex;
	}
}

void RBX::Render::Mesh::freeVertex(uint32 index)
{
	Mesh* global = getGlobalMesh();

	if (!global->varDirty)
		global->varDirty = 1;

	//removerHelper<Vector3>(index, global->vertexArray);
	////removerHelper<Vector3>(index, global->originVertexArray);
	//removerHelper<Vector3>(index, global->normalArray);
	//removerHelper<Vector2>(index, global->texCoordArray);
	//removerHelper<uint32>(index, global->vertexRefCounts);
	//global->vertexRefCount--;

}

void RBX::Render::Mesh::applyTextureUnits(RenderDevice* renderDevice)
{
}

void RBX::Render::Mesh::Level::addIndex(uint32 index)
{
	int i = indexArray.findIndex(index);
	if (i == -1)
	{
		indexArray.append(index);
	}
}

void RBX::Render::Mesh::Level::removeIndex(uint32 index)
{
	int i = indexArray.findIndex(index);
	if (i != -1)
	{
		indexArray.remove(i);
	}
}

void RBX::Render::Mesh::makeVAR()
{
	Mesh* global = getGlobalMesh();
	if (global->varDirty)
	{
		global->worldVisibleGeometry = VARArea::create(global->size() * 256, VARArea::WRITE_EVERY_FRAME);

		global->vertexVAR = VAR(global->vertexArray, global->worldVisibleGeometry);
		global->normalVAR = VAR(global->normalArray, global->worldVisibleGeometry);
		global->texCoordVAR = VAR(global->texCoordArray, global->worldVisibleGeometry);
		global->colorVAR = VAR(global->colorArray, global->worldVisibleGeometry);

		global->varDirty = 0;
	}
}

void RBX::Render::Mesh::beginRender(RenderDevice* renderDevice)
{
	Mesh* global = getGlobalMesh();

	/* make VARs */

	makeVAR();

	renderDevice->beginIndexedPrimitives();

	if (global->texCoordVAR.valid())
	{
		renderDevice->setTexCoordArray(0, global->texCoordVAR);
		//renderDevice->setVertexAttribArray(10, global->texCoordVAR, true);
	}

	if (global->vertexVAR.valid())
	{
		renderDevice->setVertexArray(global->vertexVAR);
		//renderDevice->setVertexAttribArray(0, global->vertexVAR, false);
	}

	if (global->normalVAR.valid())
	{
		renderDevice->setNormalArray(global->normalVAR);
		//renderDevice->setVertexAttribArray(2, global->normalVAR, false);
	}

	/* Remove this soon */
	if (global->colorVAR.valid())
	{
		//renderDevice->setColorArray(global->colorVAR);
		//renderDevice->setVertexAttribArray(3, global->colorVAR, false);
	}

}

void RBX::Render::Mesh::sendGeometry(Mesh::Level* level, RenderDevice* renderDevice)
{
	Mesh* global = getGlobalMesh();

	if (level && level->indexArray.size() > 0 && global->vertexRefCounts.size() > 0)
	{
		renderDevice->sendIndices(level->primitive, level->indexArray);
	}
}

void RBX::Render::Mesh::endRender(RenderDevice* renderDevice)
{
	renderDevice->endIndexedPrimitives();
}

void RBX::Render::Mesh::cleanup()
{

	Mesh* global = getGlobalMesh();

	global->texCoordArray.clear();
	global->vertexArray.clear();
	global->normalArray.clear();
	global->colorArray.clear();

	delete &global->vertexVAR;
	delete &global->normalVAR;
	delete &global->texCoordVAR;
	delete &global->colorVAR;

}

RBX::Render::Mesh* RBX::Render::Mesh::getGlobalMesh()
{
	return Datamodel::get()->globalMesh;
}