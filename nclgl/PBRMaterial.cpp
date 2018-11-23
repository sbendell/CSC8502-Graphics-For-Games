#include "PBRMaterial.h"

PBRMaterial::PBRMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum)
	: Material(Shader, Colour, Textures, TextureNum)
{

}

PBRMaterial::PBRMaterial()
{
}


PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::LoadParameters() {
	glUseProgram(shader->GetProgram());
	glUniform4f(glGetUniformLocation(shader->GetProgram(),
		"tintColour"), colour.x, colour.y, colour.z, colour.w);

	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"specularTex"), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"metalnessTex"), 3);
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void PBRMaterial::UnloadParameters() {
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}