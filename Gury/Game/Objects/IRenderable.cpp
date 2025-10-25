#include "IRenderable.h"
#include "decal.h"

#include "../Gury/Game/Rendering/renderScene.h"

void RBX::Render::IRenderable::addToProxy(Render::Mesh* mesh)
{
    Render::Mesh::Level* thisProxy = mesh->getLevel();

    for (int f = 0; f < meshIndices.size(); f++)
    {
        uint32 index = meshIndices[f];
        thisProxy->addIndex(index);
    }
}

void RBX::Render::IRenderable::removeFromProxy(Render::Mesh* mesh)
{
    Render::Mesh::Level* thisProxy = mesh->getLevel();

    for (int f = 0; f < meshIndices.size(); f++)
    {
        uint32 index = meshIndices[f];
        thisProxy->removeIndex(index);
    }
}

void RBX::Render::IRenderable::editMeshPosition(CoordinateFrame newPosition)
{
    Render::Mesh* mesh = Render::Mesh::getGlobalMesh();

    for (int f = 0; f < meshIndices.size(); f++)
    {
        uint32 index = meshIndices[f];
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

void RBX::Render::IRenderable::removeFromRenderEnvironment()
{
    Render::Mesh* mesh = Render::Mesh::getGlobalMesh();
    Render::RenderScene* scene = RBX::Render::RenderScene::get();

    if (currentProxy)
    {
        removeFromProxy(currentProxy);
    }

    for (int i = 0; i < meshIndices.size(); i++)
    {
        uint32 index = meshIndices[i];
        mesh->freeVertex(index);
    }

    meshIndices.clear();

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

    if (meshIndices.size() == 0)
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

    removeFromProxy(level);
    addToProxy(level);
}