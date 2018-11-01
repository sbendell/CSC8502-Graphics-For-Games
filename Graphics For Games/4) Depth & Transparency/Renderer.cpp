#include "Renderer.h"



Renderer::Renderer(Window& parent):
	OGLRenderer(parent)
{
	meshes[0] = Mesh::GenerateQuad();
	meshes[1] = Mesh::GenerateTriangle();
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(0.0f, 0.0f, 10.0f), 0.01f);

	currentShader = new Shader("../../Shaders/TexturedVertex.glsl",
		"../../Shaders/TexturedFragment.glsl");

	meshes[0]->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	meshes[1]->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	if (!meshes[0]->GetTexture() || !meshes[1]->GetTexture()) {
		return;
	}

	if (!currentShader->LinkProgram()) {
		return;
	}

	positions[0] = Vector3(0, 0, -5); //5 units away from the viewpoint
	positions[1] = Vector3(0, 0, -5);

	init = true;

	filtering = true;
	repeating = false;

	usingDepth = false;
	usingAlpha = false;
	blendMode = 0;
	modifyObject = true;

	projMatrix = Matrix4::Perspective(1.0f, 100.0f,
		(float)width / (float)height, 45.0f);
}


Renderer::~Renderer()
{
	delete meshes[0];
	delete meshes[1];
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());

	//UpdateShaderMatrices();
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"textureMatrix"), 1, false, (float*)&textureMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)&viewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&projMatrix);
	
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	for (unsigned int i = 0; i < 2; ++i) {
		glUniformMatrix4fv(glGetUniformLocation(
			currentShader->GetProgram(), "modelMatrix"), 1, false,
			(float*)& Matrix4::Translation(positions[i]));
		glBindTexture(GL_TEXTURE_2D, meshes[i]->GetTexture());
		meshes[i]->Draw();
	}
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, fov);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f,
		width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::UpdateTextureMatrix(float value) {
	Matrix4 pushPos = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 popPos = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pushPos * rotation * popPos;
}

void Renderer::ToggleRepeating(Mesh* mesh) {
	repeating = !repeating;
	glBindTexture(GL_TEXTURE_2D, mesh->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, //x axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, //y axis
		repeating ? GL_REPEAT : GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::ToggleFiltering(Mesh* mesh) {
	filtering = !filtering;
	glBindTexture(GL_TEXTURE_2D, mesh->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		filtering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::ToggleObject() {
	modifyObject = !modifyObject;
}

void Renderer::MoveObject(float by) {
	positions[(int)modifyObject].z += by;
}

void Renderer::ToggleDepth() {
	usingDepth = !usingDepth;
	usingDepth ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Renderer::ToggleAlphaBlend() {
	usingAlpha = !usingAlpha;
	usingAlpha ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Renderer::ToggleBlendMode() {
	blendMode = (blendMode + 1) % 4;
		switch (blendMode) {
		case (0): glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case (1): glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR); break;
		case (2): glBlendFunc(GL_ONE, GL_ZERO); break;
		case (3): glBlendFunc(GL_SRC_ALPHA, GL_ONE); break;
	};
}