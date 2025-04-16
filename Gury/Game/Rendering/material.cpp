#include "material.h"

void RBX::Render::Material::appendLevel(Color3 color, Color3 specular, float shiny, float reflect, float transparent)
{
	levels.append(new Level(color, specular, shiny, reflect, transparent));
}

void RBX::Render::Material::appendEmptyLevel()
{
	levels.append(new Level());
}

RBX::Render::Material::Level* RBX::Render::Material::getVeryTransparentLevel()
{
	for (int i = 0; i < levels.size(); i++)
	{
		if (veryTransparent(i))
		{
			return levels[i];
		}
	}
	return 0;
}

RBX::Render::Material::Level* RBX::Render::Material::getVeryReflectantLevel()
{
	for (int i = 0; i < levels.size(); i++)
	{
		if (veryReflectant(i))
		{
			return levels[i];
		}
	}
	return 0;
}

RBX::Render::Material::Level* RBX::Render::Material::getVeryOpaqueLevel()
{
	for (int i = 0; i < levels.size(); i++)
	{
		if (veryOpaque(i))
		{
			return levels[i];
		}
	}
	return 0;
}

bool RBX::Render::Material::veryTransparent(int level)
{
	return levels[level]->mTransparent > 0;
}

bool RBX::Render::Material::veryReflectant(int level)
{
	return levels[level]->mTransparent > 0;
}

bool RBX::Render::Material::veryOpaque(int level)
{
	return levels[level]->mTransparent == 0 && levels[level]->mReflect == 0;
}

void RBX::Render::Material::Level::configureRenderDevice(RenderDevice* renderDevice)
{
	renderDevice->setShininess(mShiny);
	renderDevice->setSpecularCoefficient(mSpecular);
}

void RBX::Render::Material::Level::baseTexture(RenderDevice* renderDevice)
{
}

void RBX::Render::Material::Level::matte(RenderDevice* renderDevice)
{
}
