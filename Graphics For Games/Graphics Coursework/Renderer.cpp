#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	textures.reserve(100);
	shaders.reserve(20);
	materials.reserve(50);

	unsigned int* texes = new unsigned int[2] { LoadTexture("Barren Reds", "Barren Reds.JPG"),
		LoadTexture("Barren Reds Normal", "Barren RedsDOT3.JPG") };
	Shader* terrainShader = LoadShader("Terrain Shader", "TexturedVertex.glsl", "TexturedFragment.glsl");
	Material* terrainMaterial = LoadMaterial("Rocky Terrain", terrainShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f), &texes[0], 1);
	Material* normalMat = LoadMaterial("Normal Terrain", terrainShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f), &texes[1], 1);

	Scene* newScene = new Scene(this, width, height);
	scenes.push_back(newScene);

	if (!terrainShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	for (int i = 0; i < textures.size(); i++)
	{
		if (!textures[i].second) {
			return;
		}
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init = true;
}

Renderer ::~Renderer(void) {
	for (int i = 0; i < scenes.size(); i++) { delete scenes[i]; }
	for (int i = 0; i < shaders.size(); i++) { delete shaders[i].second; }
	for (int i = 0; i < materials.size(); i++) { delete materials[i].second; }
	for (int i = 0; i < textures.size(); i++) { glDeleteTextures(1, &(textures[i].second)); }
}

void Renderer::UpdateScene(float msec) {
	for (int i = 0; i < scenes.size(); i++)
	{
		scenes[i]->UpdateScene(msec);
	}
}

void Renderer::RenderScene() {
	SetCurrentShader(shaders[0].second);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < scenes.size(); i++)
	{
		scenes[i]->RenderScene();
	}

	glUseProgram(0);
	SwapBuffers();
}