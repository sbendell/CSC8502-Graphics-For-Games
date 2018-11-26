#include "TerrainPBRMaterial.h"

TerrainPBRMaterial::TerrainPBRMaterial(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum)
	: Material(Shader, Colour, Textures, TextureNum)
{

}

TerrainPBRMaterial::TerrainPBRMaterial()
{
}


TerrainPBRMaterial::~TerrainPBRMaterial()
{
}

void TerrainPBRMaterial::LoadParameters() {
	glUseProgram(shader->GetProgram());
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"specularTex"), 2);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"rdiffuseTex"), 3);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"rbumpTex"), 4);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"rspecularTex"), 5);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"sdiffuseTex"), 6);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"sbumpTex"), 7);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"sspecularTex"), 8);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"ldiffuseTex"), 9);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"lbumpTex"), 10);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"lspecularTex"), 11);
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"craterTex"), 12);
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}

void TerrainPBRMaterial::UnloadParameters() {
	for (int i = 0; i < textureNum; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}