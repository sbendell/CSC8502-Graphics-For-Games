#pragma once
#include "OGLRenderer.h"

class Shader;

enum MaterialTypes {
	TEXTURED, BUMP,
	PBR, TERRAINPBR
};

class Material {
public:
	Material(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum);
	Material();
	~Material();

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 Colour) { colour = Colour; }

	virtual void LoadParameters();
	virtual void UnloadParameters();

	Shader* GetShader() const { return shader; }

	Matrix4 GetTextureMatrix() { return textureMatrix; }
	void SetTextureMatrix(Matrix4 mat) { textureMatrix = mat; }

protected:
	Shader* shader;
	Vector4 colour;

	Matrix4 textureMatrix;

	int textureNum;
	unsigned int textures[16];
};