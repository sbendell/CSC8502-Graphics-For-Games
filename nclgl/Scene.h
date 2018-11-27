#pragma once

#include "../Graphics For Games/Graphics Coursework/Renderer.h"
#include "Camera.h"
#include "SceneNode.h"
#include "Frustum.h"
#include "HeightMap.h"
#include "Material.h"
#include "MD5Node.h"
#include "MD5FileData.h"
#include <algorithm>

#define LIGHTNUMS1 2
#define LIGHTNUMS2 6
#define SHADOWSIZE 2048

class Renderer;
class OBJMesh;

class Scene
{
public:
	Scene(Renderer* rend, int width, int height, OBJMesh* sphere, int scene);
	Scene();
	~Scene();

	void UpdateScene(float msec);
	void RenderScene(Mesh* screen, Mesh* fullScreen);

	GLuint GetSkybox() const { return skybox; }
	void SetSkybox(GLuint val) { skybox = val; }

	Camera* GetCamera() { return camera; }

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes(bool shadow);
	void DrawNode(SceneNode* n, bool shadow);
	
	void DrawShadowScene();
	void DrawSkybox(Mesh* screen);
	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(Mesh* screen); // Combination Render Pass
	void PostProcess(Mesh* screen);
	void PresentScene(Mesh* screen);

	void GenBuffers(int lights);

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

	Shader* shadowShader;
	Shader* combineShader;
	Shader* vignetteShader;
	Shader* chrabShader;
	Shader* grayscaleShader;
	Shader* presentShader;

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	GLuint bufferFBO; // FBO for our G- Buffer pass
	GLuint bufferColourTex; // Albedo goes here
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex; // Depth goes here
	GLuint bufferSpecularTex;

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex; // Store specular lighting

	GLuint shadowFBO;
	GLuint* shadowTex;
	Matrix4* shadowMatrixes;

	GLuint postProcessFBO;
	GLuint postProcessTex[2];

	int scene;
	OBJMesh* meteorMesh;
	float ballPosition = 0;
	float cameraPos = 0;
	int postProcess = 1;
	bool drawShadows = true;
};