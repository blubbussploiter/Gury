#pragma once

#include "material.h"
#include "renderMesh.h"

namespace RBX
{
	namespace Render
	{
		class RenderSurface
		{
		public:

			Material::Level* material;
			float polygonOffset;
			Mesh* fullMesh;

		public:

			RenderSurface() : RenderSurface(0.0f){
			}

			RenderSurface(float polygonOffset) : RenderSurface(new Mesh(), new Material::Level(), polygonOffset){
			}

			RenderSurface(Material::Level* material) : RenderSurface(new Mesh(), material, 0.0f) {
			}

			RenderSurface(Material::Level* material, float polygonOffset) : RenderSurface(new Mesh(), material, polygonOffset){
			}

			RenderSurface(Mesh* mesh, Material::Level* material, float polygonOffset) : fullMesh(mesh), material(material), polygonOffset(polygonOffset){
			}
		};
	}
}