#pragma once
#include "OGLRenderer.h"

class Shader;

class Material {
public:
	Material(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum);
	Material();
	~Material();

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 Colour) { colour = Colour; }

	virtual void LoadParameters();
	virtual void UnloadParamters();

	Shader* GetShader() const { return shader; }

	Matrix4 GetTextureMatrix() { return textureMatrix; }
	void SetTextureMatrix(Matrix4 mat) { textureMatrix = mat; }

	unsigned int textures[16];
protected:
	Shader* shader;
	Vector4 colour;

	Matrix4 textureMatrix;
};