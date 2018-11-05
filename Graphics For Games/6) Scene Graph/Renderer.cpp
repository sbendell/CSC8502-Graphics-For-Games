#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube(); // Important !
	camera = new Camera();

	currentShader = new Shader(SHADERDIR "SceneVertex.glsl",
		SHADERDIR "SceneFragment.glsl");

	if (!currentShader->LinkProgram()) {
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
		(float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0, 30, 175));

	root = new SceneNode();
	for (int i = 0; i < 50; i++)
	{
		for (int x = 0; x < 50; x++)
		{
			root->AddChild(new CubeRobot());
			auto robot = root->GetChildIteratorStart() + x + (50 * i);
			(*robot)->SetTransform(Matrix4::Translation(Vector3(40 * x , 0, 40 * i)));
		}
	}

	glEnable(GL_DEPTH_TEST);
	init = true;

}

Renderer ::~Renderer(void) {
	delete root;
	CubeRobot::DeleteCube(); // Also important !
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	root->Update(msec);
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