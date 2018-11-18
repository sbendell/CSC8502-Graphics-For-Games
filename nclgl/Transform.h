#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Transform
{
public:
	Transform(Vector3 Translation, Vector3 Rotation, Vector3 Scale) {
		translation = Translation;
		rotation = Rotation;
		scale = Scale;

		localMatrix.ToIdentity();
		worldMatrix.ToIdentity();
	}
	Transform() {
		translation = Vector3(0.0f, 0.0f, 0.0f);
		rotation = Vector3(0.0f, 0.0f, 0.0f);
		scale = Vector3(1.0f, 1.0f, 1.0f);

		localMatrix.ToIdentity();
		worldMatrix.ToIdentity();
	}
	~Transform() {};

	Vector3 GetTranslation() const { return translation; }
	void SetTranslation(Vector3 Translate) { translation = Translate; }

	Vector3 GetRotation() const { return rotation; }
	void SetRotation(Vector3 Rotate) { rotation = Rotate; }

	Vector3 GetScale() const { return scale; }
	void GetScale(Vector3 Scale) { scale = Scale; }

	Matrix4 GetLocalMatrix() const { return localMatrix; }
	void SetLocalMatrix(Matrix4 mat) { localMatrix = mat; }

	Matrix4 GetWorldMatrix() const { return worldMatrix; }

	void Update(const Matrix4& parentTransform) {
		localMatrix = Matrix4::Translation(translation)
			//* Matrix4::Rotation(rotation)
			* Matrix4::Scale(scale);

		worldMatrix = parentTransform * localMatrix;
	}

protected:
	Vector3 translation;
	Vector3 rotation;
	Vector3 scale;

	Matrix4 localMatrix;
	Matrix4 worldMatrix;
};

