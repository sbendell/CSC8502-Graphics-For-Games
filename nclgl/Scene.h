#pragma once

#include "../Graphics For Games/Graphics Coursework/Renderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include <algorithm>

class Renderer;
class OBJMesh;

class Scene
{
public:
	Scene(Renderer* rend, int width, int height, OBJMesh* sphere);
	Scene();
	~Scene();

	void UpdateScene(float msec);
	void RenderScene(Mesh* screen, Mesh* fullScreen);

	GLuint GetSkybox() const { return skybox; }
	void SetSkybox(GLuint val) { skybox = val; }

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);
	
	void DrawSkybox(Mesh* screen);
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(Mesh* screen); // Combination Render Pass

	void GenBuffers();

	Renderer* renderer;

	SceneNode* root;
	Camera* camera;
	HeightMap* heightMap;
	GLuint skybox;
	Shader* skyboxShader;
	Vector3 ambientColour;

	Light* lights;
	Shader* pointLightShader;
	OBJMesh* lightSphere;

	Shader* combineShader;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here
	GLuint bufferSpecularTex;
	GLuint bufferMetalnessTex;
	GLuint bufferFragPositions;

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting
};