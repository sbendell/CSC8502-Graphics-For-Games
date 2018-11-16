#pragma once
#include "OGLRenderer.h"

class Shader;

class Material {
public:
	Material(Shader* Shader, unsigned int* Textures, int TextureNum);
	Material();
	~Material();

	virtual void LoadParameters();
	virtual void UnloadParamters();

	Shader* shader;
	unsigned int textures[16];
};