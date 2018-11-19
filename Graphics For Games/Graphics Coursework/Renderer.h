#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Material.h"
#include <algorithm>

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();

	inline Shader* GetShaderWithName(string name) {
		for (int i = 0; i < shaders.size(); i++)
		{
			if (shaders[i].first == name) return shaders[i].second;
		}
	}

	inline Material* GetMaterialWithName(string name) {
		for (int i = 0; i < materials.size(); i++)
		{
			if (materials[i].first == name) return materials[i].second;
		}
	}

	inline GLuint GetTextureWithName(string name) {
		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i].first == name) return textures[i].second;
		}
	}

	inline Shader* LoadShader(string ShaderName, string VertexPath, string FragmentPath) {
		Shader* terrainShader = new Shader(SHADERDIR + VertexPath,
			SHADERDIR + FragmentPath);
		shaders.push_back(make_pair(ShaderName, terrainShader));
		return terrainShader;
	}

	inline unsigned int LoadTexture(string TextureName, string TexturePath) {
		unsigned int texInt = SOIL_load_OGL_texture(
			(TEXTUREDIR + TexturePath).c_str(),
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
		textures.push_back(make_pair(TextureName, texInt));
		SetTextureRepeating(texInt, true);
		return texInt;
	}

	inline Material* LoadMaterial(string MaterialName, Shader* Shader, Vector4 Colour, unsigned int* Textures, int NumTextures) {
		Material* material = new Material(Shader, Colour, Textures, NumTextures);
		materials.push_back(make_pair(MaterialName, material));
		return material;
	}

protected:
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode* n);

	SceneNode* root;
	Camera* camera;
	HeightMap* heightMap;

	vector<pair<string, Shader*>> shaders;
	vector<pair<string, Material*>> materials;
	vector<pair<string, GLuint>> textures; // First = texture name  Second = texture GLuint

	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	bool mat = false;
};