#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light(Vector3 position, Vector4 colour, float radius, float brightness) {
		this->position = position;
		this->colour = colour;
		this->radius = radius;
		this->brightness = brightness;
	}
	Light() {};
	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 val) { colour = val; }

	float GetBrightness() const { return brightness; }
	void SetBrightness(float val) { brightness = val; }

protected:
	Vector3 position;
	Vector4 colour;
	float radius;
	float brightness;
};