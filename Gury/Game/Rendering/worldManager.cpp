#include "worldManager.h"
#include "textureReserve.h"

using namespace RBX;

Render::WorldManager* worldManager = new Render::WorldManager();

void RBX::Render::WorldManager::step()
{
    if (dirty)
    {
        TextureReserve::get()->regenWorld();
        dirty = false;
    }
}

void RBX::Render::WorldManager::makeDirty()
{
    if (!dirty)
    {
        dirty = true;
    }
}

Render::WorldManager* Render::WorldManager::get()
{
    return worldManager;
}
