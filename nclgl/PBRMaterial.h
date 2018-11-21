#pragma once

#include "../../nclgl/Material.h"

class PBRMaterial : public Material
{
public:
	PBRMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum);
	PBRMaterial();
	~PBRMaterial();

	void LoadParameters() override;
	void UnloadParameters() override;
};

