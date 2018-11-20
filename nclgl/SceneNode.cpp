#include "SceneNode.h"

SceneNode::SceneNode(Mesh* mesh) {
	this->mesh = mesh;
	parent = NULL;
	material = NULL;

	boundingRadius = 1.0f;
	distanceFromCamera = 0.0f;
}

SceneNode ::~SceneNode(void) {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

void SceneNode::AddChild(SceneNode* s) {
	children.push_back(s);
	s->parent = this;
	s->transform.Update(this->transform.GetWorldMatrix());
}

void SceneNode::Draw() {
	if (mesh) {
		material->LoadParameters();
		mesh->Draw();
		material->UnloadParamters();
	}
}

void SceneNode::Update(float msec) {
	if (parent) {
		transform.Update(parent->transform.GetWorldMatrix());
	}

	for (vector<SceneNode*>::iterator i = children.begin(); i != children.end(); ++i) {
		(*i)->Update(msec);
	}
}
