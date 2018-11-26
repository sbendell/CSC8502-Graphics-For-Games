#pragma once

#include "../../nclgl/Material.h"

class TerrainPBRMaterial : public Material
{
public:
	TerrainPBRMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum);
	TerrainPBRMaterial();
	~TerrainPBRMaterial();

	void LoadParameters() override;
	void UnloadParameters() override;
};

