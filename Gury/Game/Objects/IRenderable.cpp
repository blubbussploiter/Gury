#include "IRenderable.h"
#include "decal.h"

#include "../Gury/Game/Rendering/renderScene.h"

void RBX::Render::IRenderable::addVertexIndicesToProxy(Render::Mesh* mesh, Table<NormalId, Face> vIndices)
{
    auto& keys = vIndices.getKeys();

    for (int i = 0; i < keys.size(); i++)
    {
        NormalId key = keys[i];
        Face face = vIndices[key];

        bool textured = face.textured;

        Render::Mesh::Level* thisProxy = mesh->getLevel();

        for (int f = 0; f < face.indices.size(); f++)
        {
            uint32 index = face.indices[f];
            thisProxy->addIndex(index);
        }
    }
}

void RBX::Render::IRenderable::removeVertexIndicesFromProxy(Render::Mesh* mesh, Table<NormalId, Face> vIndices)
{
    auto& keys = vIndices.getKeys();
    for (int i = 0; i < keys.size(); i++)
    {
        NormalId key = keys[i];
        auto& face = vIndices[key];

        Render::Mesh::Level* thisProxy = mesh->getLevel();

        for (int f = 0; f < face.indices.size(); f++)
        {
            uint32 index = face.indices[f];
            thisProxy->removeIndex(index);
        }
    }
}

void RBX::Render::IRenderable::removeFaceFromProxy(Render::Mesh* mesh, Face face)
{
    Render::Mesh::Level* thisProxy = mesh->getLevel();

    for (int f = 0; f < face.indices.size(); f++)
    {
        uint32 index = face.indices[f];
        thisProxy->removeIndex(index);
    }
}

void RBX::Render::IRenderable::addFaceToProxy(Render::Mesh* proxy, Face face)
{
    Render::Mesh::Level* thisProxy = proxy->getLevel();

    for (int f = 0; f < face.indices.size(); f++)
    {
        uint32 index = face.indices[f];
        thisProxy->addIndex(index);
    }
}

void RBX::Render::IRenderable::addToProxy(Render::Mesh* proxy)
{
    addVertexIndicesToProxy(proxy, vertexIndices);
}

void RBX::Render::IRenderable::removeFromProxy(Render::Mesh* proxy)
{
    removeVertexIndicesFromProxy(proxy, vertexIndices);
}

void RBX::Render::IRenderable::editMeshPosition(CoordinateFrame newPosition)
{
    auto& keys = vertexIndices.getKeys();
    Render::Mesh* mesh = Render::Mesh::getGlobalMesh();

    for (int i = 0; i < keys.size(); i++)
    {
        NormalId key = keys[i];
        auto& face = vertexIndices[key];

        for (int f = 0; f < face.indices.size(); f++)
        {
            uint32 index = face.indices[f];
            Color4 color;
            Vector3 oldVertex, newVertex, normal;
            Vector2 uv;

            oldVertex = mesh->vertexArray[index];
            newVertex = newPosition.pointToWorldSpace(oldVertex);

            normal = mesh->normalArray[index];
            uv = mesh->texCoordArray[index];

            color = mesh->colorArray[index];

            Render::Mesh::edit(index, newVertex, normal, uv);
        }
    }
}

void RBX::Render::IRenderable::removeFromRenderEnvironment()
{
    Render::Mesh* mesh = Render::Mesh::getGlobalMesh();
    Array<NormalId> keys = vertexIndices.getKeys();

    Render::RenderScene* scene = RBX::Render::RenderScene::get();

    if (currentProxy)
    {
        removeFromProxy(currentProxy);
    }

    if (vertexIndices.containsKey(Front))
    {
        vertexIndices.remove(Front);
    }
    if (vertexIndices.containsKey(Back))
    {
        vertexIndices.remove(Back);
    }
    if (vertexIndices.containsKey(Right))
    {
        vertexIndices.remove(Right);
    }
    if (vertexIndices.containsKey(Left))
    {
        vertexIndices.remove(Left);
    }
    if (vertexIndices.containsKey(Top))
    {
        vertexIndices.remove(Top);
    }
    if (vertexIndices.containsKey(Bottom))
    {
        vertexIndices.remove(Bottom);
    }
    if (vertexIndices.containsKey(UNDEFINED))
    {
        vertexIndices.remove(UNDEFINED);
    }

    if (specialShape) {
        specialShape->removeFromRenderEnvironment();
    }

    currentProxy = 0;

}

void RBX::Render::IRenderable::editGlobalProxyLocation()
{
    Render::RenderScene* scene = RBX::Render::RenderScene::get();

    Render::RenderSurface* opaque = scene->proxy;
    Render::RenderSurface* reflect = scene->reflectProxy;
    Render::RenderSurface* transparent = scene->transparentProxy;
    Render::RenderSurface* textured = scene->texturedProxy;

    Render::Mesh* proxy;

    if (vertexIndices.size() == 0)
    {
        return;
    }

    if (reflectance > 0) // Reflecting
    {
        proxy = reflect->fullMesh;
    }
    else if (transparency > 0) // Transparent
    {
        proxy = transparent->fullMesh;
    }
    else // Opaque 
    {
        proxy = opaque->fullMesh;
    }

    if (proxy)
    {
        if (currentProxy != proxy)
        {
            if (currentProxy)
            {
                removeFromProxy(currentProxy);
            }
            addToProxy(proxy);
            currentProxy = proxy;
        }
    }

}

void  RBX::Render::IRenderable::editGlobalTexturedProxyLocation()
{
    Render::RenderScene* scene = RBX::Render::RenderScene::get();
    Render::RenderSurface* textured = scene->texturedProxy;
    Render::Mesh* level = textured->fullMesh;

    removeVertexIndicesFromProxy(level, vertexIndices);
    addVertexIndicesToProxy(level, vertexIndices);
}