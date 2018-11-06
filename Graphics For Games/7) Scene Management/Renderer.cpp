#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube(); // Important !
	camera = new Camera();
	camera->SetPosition(Vector3(0, 100, 750.0f));

	currentShader = new Shader(SHADERDIR "SceneVertex.glsl",
		SHADERDIR "SceneFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	quad = Mesh::GenerateQuad();
	quad->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	root = new SceneNode();
	for (int i = 0; i < 5; ++i) {
		SceneNode * s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(
		Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(quad);
		root->AddChild(s);
	}
	root->AddChild(new CubeRobot());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	init = true;
}

Renderer ::~Renderer(void) {
	delete root;
	CubeRobot::DeleteCube(); // Also important !
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	//root->Update(msec);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 1);

	DrawNode(root);

	glUseProgram(0);
	SwapBuffers();
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
	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n-> GetChildIteratorEnd(); ++i) {
		DrawNode(*i);
	}
}