#pragma once

#include "../Graphics For Games/Graphics Coursework/Renderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include <algorithm>

class Renderer;

class Scene
{
public:
	Scene(Renderer* rend, int width, int height);
	Scene();
	~Scene();

	void UpdateScene(float msec);
	void RenderScene();

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	Renderer* renderer;

	SceneNode* root;
	Camera* camera;
	HeightMap* heightMap;

	Matrix4 projMatrix;
	Matrix4 viewMatrix;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};

