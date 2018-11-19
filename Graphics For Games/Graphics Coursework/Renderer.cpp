#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	textures.reserve(100);
	shaders.reserve(20);
	materials.reserve(50);
	camera = new Camera(0.0f, -135.0f, 0.0f, Vector3(0, 500, 0), 1.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	unsigned int texture = LoadTexture("Barren Reds", "Barren Reds.JPG");
	unsigned int normal = LoadTexture("Barren Reds Normal", "Barren RedsDOT3.JPG");
	Shader* terrainShader = LoadShader("Terrain Shader", "TexturedVertex.glsl", "TexturedFragment.glsl");

	Material* material = new Material(terrainShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f), &texture, 1);
	materials.push_back(make_pair("Rocky Terrain", material));

	Material* normalMat = new Material(terrainShader, Vector4(1.0f, 1.0f, 1.0f, 1.0f), &normal, 1);
	materials.push_back(make_pair("Normal Terrain", normalMat));

	if (!terrainShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	root = new SceneNode();

	SceneNode* terrain = new SceneNode(heightMap);
	terrain->SetMaterial(materials[0].second);
	terrain->SetBoundingRadius(100000.0f);
	root->AddChild(terrain);

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
	delete root;
	delete camera;
	delete heightMap;
	for (int i = 0; i < shaders.size(); i++) { delete shaders[i].second; }
	for (int i = 0; i < materials.size(); i++) { delete materials[i].second; }
	for (int i = 0; i < textures.size(); i++) { glDeleteTextures(1, &(textures[i].second)); }
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix*viewMatrix);
	root->Update(msec);
	auto hm = root->GetChildIteratorStart();
	(*hm)->GetTransform().SetRotation((*hm)->GetTransform().GetRotation() + Vector3(10.0f, 0.0f, 0.0f));
	(*hm)->SetMaterial(materials[mat].second);
	mat = !mat;
}

void Renderer::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();
	SetCurrentShader(shaders[0].second);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	UpdateShaderMatrices();

	DrawNodes();

	glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

void Renderer::BuildNodeLists(SceneNode * from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetTransform().GetWorldMatrix().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetMaterial()) {
			if (from->GetMaterial()->GetColour().w < 1.0f) {
				transparentNodeList.push_back(from);
			}
			else {
				nodeList.push_back(from);
			}
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
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),
			"modelMatrix"), 1, false, (float*)&n->GetTransform().GetWorldMatrix());

		n->Draw(*this);
	}
}