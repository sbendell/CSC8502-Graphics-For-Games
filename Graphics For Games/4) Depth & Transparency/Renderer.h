#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "../../nclgl/Camera.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

	void SwitchToPerspective();
	void SwitchToOrthographic();

	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating(Mesh* mesh);
	void ToggleFiltering(Mesh* mesh);

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);

	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	inline void SetFOV(float f) { fov = f; }
protected:
	Mesh * meshes[2];
	Vector3 positions[2];
	Camera* camera;

	bool filtering;
	bool repeating;

	Matrix4 textureMatrix;
	Matrix4 modelMatrix;
	Matrix4 projMatrix;
	Matrix4 viewMatrix;

	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;

	float scale;
	float rotation;
	Vector3 position;
	float fov;
};