#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	loadedTextures.reserve(100);
	loadedShaders.reserve(20);
	materials.reserve(50);
	camera = new Camera(0.0f, -135.0f, 0.0f, Vector3(0, 500, 0), 1.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	loadedTextures.push_back(make_pair("Barren Reds", SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)));
	SetTextureRepeating(loadedTextures[0].second, true);

	Shader* terrainShader = new Shader(SHADERDIR "TexturedVertex.glsl",
		SHADERDIR "TexturedFragment.glsl");
	loadedShaders.push_back(make_pair("Terrain Shader", terrainShader));

	unsigned int* textures = new unsigned int[16];
	textures[0] = (unsigned int)loadedTextures[0].second;
	Material* material = new Material(terrainShader, textures, 1);
	materials.push_back(material);

	if (!terrainShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	root = new SceneNode();

	for (int i = 0; i < loadedTextures.size(); i++)
	{
		if (!loadedTextures[i].second) {
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
	delete root;
	delete camera;
	delete heightMap;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix*viewMatrix);
	root->Update(msec);
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();
	SetCurrentShader(loadedShaders[0].second);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	UpdateShaderMatrices();

	heightMap->Draw(*materials[0]);

	DrawNodes();

	glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

void Renderer::BuildNodeLists(SceneNode * from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
	glDisable(GL_CULL_FACE);
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
	glEnable(GL_CULL_FACE);
}

void Renderer::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() *
			Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
			"modelMatrix"), 1, false, (float*)&transform);

		n->Draw(*this, *materials[0]);
	}
}