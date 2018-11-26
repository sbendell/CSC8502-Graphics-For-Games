#include "BumpMaterial.h"

BumpMaterial::BumpMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum)
	: Material(Shader, Colour, Textures, TextureNum)
{

}

BumpMaterial::BumpMaterial()
{
}


BumpMaterial::~BumpMaterial()
{
}

void BumpMaterial::LoadParameters() {
	glUseProgram(shader->GetProgram());
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"bumpTex"), 1);
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void BumpMaterial::UnloadParameters() {
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}