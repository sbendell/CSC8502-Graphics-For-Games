#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	textures.reserve(100);
	shaders.reserve(20);
	materials.reserve(50);

	unsigned int* terraintexes = new unsigned int[13]{
		LoadTexture("Grass1 Albedo", "Grass_001_SD/Grass_001_COLOR.jpg"),
		LoadTexture("Grass1 Normal", "Grass_001_SD/Grass_001_NORM.jpg"),
		LoadTexture("Grass1 Specular", "Grass_001_SD/Grass_001_ROUGH.jpg"),
		LoadTexture("Rock1 Albedo", "Rock_Ore_001_SD/Rock_Ore_001_COLOR.jpg"),
		LoadTexture("Rock1 Normal", "Rock_Ore_001_SD/Rock_Ore_001_NORM.jpg"),
		LoadTexture("Rock1 Specular", "Rock_Ore_001_SD/Rock_Ore_001_SPEC.jpg"),
		LoadTexture("Snow1 Albedo", "Snow_001_SD/Snow_001_COLOR.jpg"),
		LoadTexture("Snow1 Normal", "Snow_001_SD/Snow_001_NORM.jpg"),
		LoadTexture("Snow1 Specular", "Snow_001_SD/Snow_001_ROUGH.jpg"),
		LoadTexture("Meteor1 Albedo", "Lava_001_SD/Lava_001_COLOR.png"),
		LoadTexture("Meteor1 Normal", "Lava_001_SD/Lava_001_NORM.png"),
		LoadTexture("Meteor1 Specular", "Lava_001_SD/Lava_001_SPEC.png"),
		LoadTexture("Crater", "Crater.png")
	};
	unsigned int* meteortexes = new unsigned int[4]{
		LoadTexture("Meteor2 Albedo", "Lava_001_SD/Lava_001_COLOR.png"),
		LoadTexture("Meteor2 Normal", "Lava_001_SD/Lava_001_NORM.png"),
		LoadTexture("Meteor2 Specular", "Lava_001_SD/Lava_001_SPEC.png"),
		LoadTexture("Meteor2 Metalness", "black.png")
	};
	unsigned int* shinymetaltexes = new unsigned int[4]{
		LoadTexture("Metal1 Albedo", "Crater.png"),
		LoadTexture("Metal1 Normal", "Crater.png"),
		LoadTexture("Metal1 Specular", "Crater.png"),
		LoadTexture("Metal1 Metalness", "Crater.png")
	};
	unsigned int* metaldiamondtexes = new unsigned int[4]{
		LoadTexture("Metal2 Albedo", "Metal_Diamond_001_SD/Metal_Diamond_001_COLOR.jpg"),
		LoadTexture("Metal2 Normal", "Metal_Diamond_001_SD/Metal_Diamond_001_NORM.jpg"),
		LoadTexture("Metal2 Specular", "Metal_Diamond_001_SD/Metal_Diamond_001_ROUGH.jpg"),
		LoadTexture("Metal2 Metalness", "Metal_Diamond_001_SD/Metal_Diamond_001_ROUGH.jpg")
	};
	unsigned int* metalscalestexes = new unsigned int[4]{
		LoadTexture("Metal3 Albedo", "Metal_Scales_001_SD/Scales_001_COLOR.jpg"),
		LoadTexture("Metal3 Normal", "Metal_Scales_001_SD/Scales_001_NORM.jpg"),
		LoadTexture("Metal3 Specular", "Metal_Scales_001_SD/Scales_001_ROUGH.jpg"),
		LoadTexture("Metal3 Metalness", "Metal_Scales_001_SD/Scales_001_ROUGH.jpg")
	};
	unsigned int* metalscratchedtexes = new unsigned int[4]{
		LoadTexture("Metal4 Albedo", "Metal_scratched_005_SD/Metal_scratched_005_COLOR.jpg"),
		LoadTexture("Metal4 Normal", "Metal_scratched_005_SD/Metal_scratched_005_NRM.jpg"),
		LoadTexture("Metal4 Specular", "Metal_scratched_005_SD/Metal_scratched_005_SPEC.jpg"),
		LoadTexture("Metal4 Metalness", "Metal_scratched_005_SD/Metal_scratched_005_SPEC.jpg")
	};
	unsigned int* tilestexes = new unsigned int[4]{
		LoadTexture("Tiles1 Albedo", "Tiles_012_SD/Tiles_012_COLOR.jpg"),
		LoadTexture("Tiles1 Normal", "Tiles_012_SD/Tiles_012_NRM.jpg"),
		LoadTexture("Tiles1 Specular", "Tiles_012_SD/Tiles_012_ROUGH.jpg"),
		LoadTexture("Tiles1 Metalness", "Tiles_012_SD/Tiles_012_COLOR.jpg")
	};
	unsigned int* bricktexes = new unsigned int[4]{
		LoadTexture("Brick1 Albedo", "Brick_wall_007_SD/Brick_wall_007_COLOR.jpg"),
		LoadTexture("Brick1 Normal", "Brick_wall_007_SD/Brick_wall_007_NRM.jpg"),
		LoadTexture("Brick1 Specular", "Brick_wall_007_SD/Brick_wall_007_SPEC.jpg"),
		LoadTexture("Brick1 Metalness", "Brick_wall_007_SD/Brick_wall_007_SPEC.jpg")
	};

	unsigned int sOnecubeMap = LoadCubeMap("Glacier", "hw_glacier/glacier_ft.tga", "hw_glacier/glacier_bk.tga",
		"hw_glacier/glacier_up.tga", "hw_glacier/glacier_dn.tga",
		"hw_glacier/glacier_rt.tga", "hw_glacier/glacier_lf.tga");
	unsigned int sTwocubeMap = LoadCubeMap("Midnight", "mp_midnight/midnight-silence_ft.tga", "mp_midnight/midnight-silence_bk.tga",
		"mp_midnight/midnight-silence_up.tga", "mp_midnight/midnight-silence_dn.tga",
		"mp_midnight/midnight-silence_rt.tga", "mp_midnight/midnight-silence_lf.tga");
	unsigned int sThreecubeMap = LoadCubeMap("Nevada", "rusted_west.jpg", "rusted_east.jpg",
		"rusted_up.jpg", "rusted_down.jpg",
		"rusted_south.jpg", "rusted_north.jpg");

	Shader* pbrShader = LoadShader("PBR", "pbrvertex.glsl", "pbrfrag.glsl");
	Shader* terrainShader = LoadShader("Terrain", "terrainpbrvert.glsl", "terrainpbrfrag.glsl");
	Shader* pointLightShader = LoadShader("Point Light", "pointlightvert.glsl", "pointlightfrag.glsl");
	Shader* skyboxShader = LoadShader("Skybox", "skyboxVertex.glsl", "skyboxFragment2.glsl");
	Shader* combineShader = LoadShader("Combine", "combinevert.glsl", "combinefrag.glsl");
	Shader* vignetteShader = LoadShader("Vignette Post Process", "TexturedVertex.glsl", "vignettefrag.glsl");
	Shader* chrabShader = LoadShader("Chrab Post Process", "TexturedVertex.glsl", "Chrabfrag.glsl");
	Shader* grayscaleShader = LoadShader("Grayscale Post Process", "TexturedVertex.glsl", "grayscalefrag.glsl");
	Shader* textureShader = LoadShader("Texture", "TexturedVertex.glsl", "TexturedFragment.glsl");
	Shader* shadowShader = LoadShader("Shadow", "shadowvert.glsl", "shadowfrag.glsl");

	Material* terrainMaterial = LoadMaterial("Rocky Terrain", terrainShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		terraintexes, 13, TERRAINPBR);
	Material* meteorMaterial = LoadMaterial("Meteor", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		meteortexes, 4, PBR);
	Material* shinyMetalMaterial = LoadMaterial("Shiny Metal", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		shinymetaltexes, 4, PBR);
	Material* metalDiamondMaterial = LoadMaterial("Metal Diamond", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		metaldiamondtexes, 4, PBR);
	Material* metalScalesMaterial = LoadMaterial("Metal Scales", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		metalscalestexes, 4, PBR);
	Material* metalScratchedMaterial = LoadMaterial("Metal Scratched", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		metalscratchedtexes, 4, PBR);
	Material* tilesMaterial = LoadMaterial("Tiles", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		tilestexes, 4, PBR);
	Material* brickMaterial = LoadMaterial("Brick", pbrShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f),
		bricktexes, 4, PBR);

	for (int i = 0; i < shaders.size(); i++)
	{
		if (!shaders[i].second->LinkProgram()) {
			return;
		}
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	screenQuad = new Mesh[4];
	screenQuad[0] = *Mesh::GenerateScreenQuad(-1.0f, -1.0f, 0.0f, 0.0f);
	screenQuad[1] = *Mesh::GenerateScreenQuad(0.0f, -1.0f, 1.0f, 0.0f);
	screenQuad[2] = *Mesh::GenerateScreenQuad(-1.0f, 0.0f, 0.0f, 1.0f);
	screenQuad[3] = *Mesh::GenerateScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

	fullScreenQuad = Mesh::GenerateQuad();

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR "ico.obj")) {
		return;
	}

	for (int i = 0; i < textures.size(); i++)
	{
		if (!textures[i].second) {
			return;
		}
	}

	for (int i = 0; i < cubeMaps.size(); i++)
	{
		if (!cubeMaps[i].second) {
			return;
		}
	}

	Scene* sceneOne = new Scene(this, width, height, sphere, 1);
	sceneOne->SetSkybox(sOnecubeMap);
	scenes.push_back(sceneOne);

	Scene* sceneTwo = new Scene(this, width, height, sphere, 2);
	sceneTwo->SetSkybox(sTwocubeMap);
	scenes.push_back(sceneTwo);

	Scene* sceneThree = new Scene(this, width, height, sphere, 3);
	sceneThree->SetSkybox(sThreecubeMap);
	scenes.push_back(sceneThree);

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}


Renderer ::~Renderer(void) {
	for (int i = 0; i < scenes.size(); i++) { delete scenes[i]; }
	for (int i = 0; i < shaders.size(); i++) { delete shaders[i].second; }
	for (int i = 0; i < materials.size(); i++) { delete materials[i].second; }
	for (int i = 0; i < textures.size(); i++) { glDeleteTextures(1, &(textures[i].second)); }

	delete sphere;
	delete[] screenQuad;
	delete fullScreenQuad;
}

void Renderer::GenerateScreenTexture(GLuint & into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0,
		depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateScene(float msec) {

	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_RIGHT)) {
		currentScene++;
		if (currentScene > scenes.size()) {
			currentScene = 0;
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_LEFT)) {
		currentScene--;
		if (currentScene < 0) {
			currentScene = scenes.size();
		}
	}
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P)) {
		paused = !paused;
	}
	if (currentScene == scenes.size()) {
		for (int i = 0; i < scenes.size(); i++)
		{
			scenes[i]->UpdateScene(msec);
		}
	}
	else {
		scenes[currentScene]->UpdateScene(msec);
	}
	if (!paused) {
		timer++;
		if (timer > 600) {
			currentScene++;
			if (currentScene > scenes.size()) {
				currentScene = 0;
			}
			timer = 0;
		}
	}
	lastFrameFPS = 1000.0f / msec;
	cout << lastFrameFPS << endl;
}

void Renderer::RenderScene() {
	SetCurrentShader(shaders[0].second);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if (currentScene == scenes.size()) {
		for (int i = 0; i < scenes.size(); i++)
		{
			scenes[i]->RenderScene(&screenQuad[i], fullScreenQuad);
		}
	}
	else {
		scenes[currentScene]->RenderScene(fullScreenQuad, fullScreenQuad);
	}
	//Doesn't render to correct part of the screen
	//DrawTextOnScreen("Hello World", Vector3(0, 0, 0), NULL, 1.0f);

	glUseProgram(0);
	SwapBuffers();
}

void Renderer::SmashTerrain(int xPos, int yPos, GLuint texture) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture);
	float* pixels = new float[256 * 256 * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels);
	for (int i = -20; i < 20; i++)
	{
		for (int j = -20; j < 20; j++)
		{
			float temp = sqrt(pow(i, 2) + pow(j, 2));
			int index = 4 * ((xPos + i) * 256 + (yPos + j)) + 0;
			if (temp < 20) {
				pixels[index + 0] = pixels[index + 0] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 1] = pixels[index + 1] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 2] = pixels[index + 2] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 3] = 1.0f;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	delete pixels;
}

void Renderer::DrawTextOnScreen(const std::string &text, const Vector3 &position, Camera* camera,
	const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	SetCurrentShader(GetShaderWithName("Texture"));
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindTexture(GL_TEXTURE_2D, basicFont->texture);
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->GetViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		glClear(GL_DEPTH_BUFFER_BIT);
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
		textureMatrix.ToIdentity();
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}