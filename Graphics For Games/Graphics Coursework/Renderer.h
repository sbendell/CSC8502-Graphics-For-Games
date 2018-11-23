#pragma once

#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Frustum.h"
#include "../../nclgl/HeightMap.h"
#include "../../nclgl/Scene.h"
#include "../../nclgl/OBJmesh.h"
#include "TextMesh.h"
#include <algorithm>

#include "../../nclgl/Material.h"
#include "../../nclgl/BumpMaterial.h"
#include "../../nclgl/PBRMaterial.h"
#include "../../nclgl/TerrainPBRMaterial.h"

class Scene;

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
			if (textures[i].first == name)
				return textures[i].second;
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

	inline unsigned int LoadCubeMap(string TextureName, string left, string right, string up, string down, string back, string front) {
		unsigned int cubeMap = SOIL_load_OGL_cubemap(
			(TEXTUREDIR + left).c_str(), (TEXTUREDIR + right).c_str(),
			(TEXTUREDIR + up).c_str(), (TEXTUREDIR + down).c_str(),
			(TEXTUREDIR + back).c_str(), (TEXTUREDIR + front).c_str(),
			SOIL_LOAD_RGB,
			SOIL_CREATE_NEW_ID, 0
		);
		cubeMaps.push_back(make_pair(TextureName, cubeMap));
		return cubeMap;
	}

	inline Material* LoadMaterial(string MaterialName, Shader* Shader, Vector4 Colour,
		unsigned int* Textures, int NumTextures, int type) {
		switch (type) {
		case TEXTURED:
		{
			Material* material = new Material(Shader, Colour, Textures, NumTextures);
			materials.push_back(make_pair(MaterialName, material));
			return material;
		}
		case BUMP:
		{
			BumpMaterial* bumpMaterial = new BumpMaterial(Shader, Colour, Textures, NumTextures);
			materials.push_back(make_pair(MaterialName, bumpMaterial));
			return bumpMaterial;
		}
		case PBR:
		{
			PBRMaterial* pbrMaterial = new PBRMaterial(Shader, Colour, Textures, NumTextures);
			materials.push_back(make_pair(MaterialName, pbrMaterial));
			return pbrMaterial;
		}
		case TERRAINPBR:
		{
			TerrainPBRMaterial* terrainpbrMaterial = new TerrainPBRMaterial(Shader, Colour, Textures, NumTextures);
			materials.push_back(make_pair(MaterialName, terrainpbrMaterial));
			return terrainpbrMaterial;
		}
		}
	}

	void GenerateScreenTexture(GLuint & into, bool depth = false);
	void SmashTerrain(int x, int y, GLuint texture);
	void DrawTextOnScreen(const std::string &text, const Vector3 &position, Camera* camera,
		const float size = 10.0f, const bool perspective = false);

	Font* basicFont;

protected:
	vector<pair<string, Shader*>> shaders;
	vector<pair<string, Material*>> materials;
	vector<pair<string, GLuint>> textures; // First = texture name  Second = texture GLuint
	vector<pair<string, GLuint>> cubeMaps;

	Mesh* screenQuad;
	Mesh* fullScreenQuad;
	OBJMesh* sphere;

	vector<Scene*> scenes;
	int currentScene = 0;
	float lastFrameFPS;
};