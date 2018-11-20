#pragma once

#include "../../nclgl/Material.h"

class BumpMaterial : public Material
{
public:
	BumpMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum);
	BumpMaterial();
	~BumpMaterial();

	virtual void LoadParamaters();
	virtual void UnloadParamaters();
};

