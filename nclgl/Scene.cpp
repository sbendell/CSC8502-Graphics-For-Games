#include "Scene.h"

Scene::Scene(Renderer* rend, int width, int height) {
	renderer = rend;
	camera = new Camera(0.0f, -135.0f, 0.0f, Vector3(0, 500, 0), 1.0f, 1.0f, 10000.0f, width, height, 45.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	root = new SceneNode();

	SceneNode* terrain = new SceneNode(heightMap);
	terrain->SetMaterial(renderer->GetMaterialWithName("Rocky Terrain"));
	terrain->SetBoundingRadius(100000.0f);
	root->AddChild(terrain);
}

Scene::Scene()
{

}


Scene::~Scene()
{
}

void Scene::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	frameFrustum.FromMatrix(camera->GetProjectionMatrix()*camera->GetViewMatrix());
	root->Update(msec);
}

void Scene::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawNodes();
	ClearNodeLists();
}

void Scene::BuildNodeLists(SceneNode * from) {
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

void Scene::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Scene::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Scene::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
	glDisable(GL_CULL_FACE);
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
	glEnable(GL_CULL_FACE);
}

void Scene::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		GLuint shaderProgram = n->GetMaterial()->GetShader()->GetProgram();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram,
			"modelMatrix"), 1, false, (float*)&n->GetTransform().GetWorldMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, false, (float*)&camera->GetViewMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projMatrix"), 1, false, (float*)&camera->GetProjectionMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "textureMatrix"), 1, false, (float*)&n->GetMaterial()->GetTextureMatrix());

		n->Draw(*renderer);
	}
}