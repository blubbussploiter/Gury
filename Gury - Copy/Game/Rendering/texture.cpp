#include "texture.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RBX::Texture>("Texture")
         .constructor<>()
         .property("StudsPerTileU", &RBX::Texture::getStudsPerTileU, &RBX::Texture::setStudsPerTileU)(rttr::metadata("Type", RBX::Appearance))
         .property("StudsPerTileV", &RBX::Texture::getStudsPerTileV, &RBX::Texture::setStudsPerTileV)(rttr::metadata("Type", RBX::Appearance));
}

void RBX::Texture::render(RenderDevice* rd, RBX::Render::IRenderable* p)
{
    if (texture.isNull())
    {
        initContentTexture();
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->getOpenGLID());

    p->renderFaceFitForTexture(rd, face, studsPerTile);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

}
