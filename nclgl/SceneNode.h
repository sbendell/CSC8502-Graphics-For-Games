#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "Transform.h"
#include <vector>

class SceneNode {
public:
	SceneNode(Mesh* m = NULL);
	~SceneNode(void);

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	Transform& GetTransform() { return transform; }

	Material* GetMaterial() const { return material; }
	void SetMaterial(Material* mat) { material = mat; }

	void AddChild(SceneNode* s);

	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer& r);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() {
		return children.begin();
	}

	std::vector <SceneNode*>::const_iterator GetChildIteratorEnd() {
		return children.end();
	}

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	Transform transform;
	Material* material;
	std::vector<SceneNode*> children;

	float distanceFromCamera;
	float boundingRadius;
};