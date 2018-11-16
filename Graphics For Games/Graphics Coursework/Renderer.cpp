#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera(-90.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH / 2, 10.0f, RAW_HEIGHT / 2), 1.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	currentShader = new Shader(SHADERDIR "SceneVertex.glsl",
		SHADERDIR "SceneFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	root = new SceneNode();

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR"Barren Reds.JPG",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!heightMap->GetTexture()) {
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());

	heightMap->Draw();

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

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),
			"nodeColour"), 1, (float*)& n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
			"useTexture"), (int)n->GetMesh()->GetTexture());

		n->Draw(*this);
	}
}