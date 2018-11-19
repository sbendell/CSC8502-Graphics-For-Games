#include "Material.h"

Material::Material(Shader* Shader, Vector4 Colour, unsigned int* Textures, int TextureNum) {
	shader = Shader;
	colour = Colour;

	textureMatrix.ToIdentity();

	for (int i = 0; i < TextureNum; i++)
	{
		textures[i] = Textures[i];
	}
}

Material::Material()
{
}


Material::~Material()
{
}

void Material::LoadParameters() {
	glUseProgram(shader->GetProgram());
	glUniform1i(glGetUniformLocation(shader->GetProgram(),
		"diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
};

void Material::UnloadParamters() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
};