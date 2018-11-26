/******************************************************************************
Class:Camera
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:FPS-Style camera. Uses the mouse and keyboard from the Window
class to get movement values!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Window.h"
#include "Matrix4.h"
#include "Vector3.h"

class Camera	{
public:
	Camera(void){
		yaw		= 0.0f;
		pitch	= 0.0f;
		roll = 0.0f;
		speed = 0.1f;

		windowWidth = 800;
		windowHeight = 600;

		projectionMatrix = Matrix4::Perspective(1.0f, 10000.0f,
			(float)800 / (float)600, 45.0f);
	};

	Camera(float pitch, float yaw, float roll, Vector3 position, float speed, float nearPlane,
		float farPlane, int windowWidth, int windowHeight, float fov) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
		this->position = position;
		this->speed = speed;
		this->nearPlane = nearPlane;
		this->farPlane = farPlane;
		this->fov = fov;
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;

		BuildProjectionMatrix();
	}

	~Camera(void){};

	void UpdateCamera(float msec = 10.0f);

	Matrix4 GetViewMatrix() const { return viewMatrix; }

	Matrix4 GetProjectionMatrix() const { return projectionMatrix; }

	//Gets position in world space
	Vector3 GetPosition() const { return position;}
	//Sets position in world space
	void	SetPosition(Vector3 val) { position = val;}

	//Gets yaw, in degrees
	float	GetYaw()   const { return yaw;}
	//Sets yaw, in degrees
	void	SetYaw(float y) {yaw = y;}

	//Gets pitch, in degrees
	float	GetPitch() const { return pitch;}
	//Sets pitch, in degrees
	void	SetPitch(float p) {pitch = p;}

	float GetRoll() const { return roll; }
	void SetRoll(float r) { roll = r; }

	float GetSpeed() const { return speed; }
	void SetSpeed(float s) { speed = s; }

	float GetNearPlane() const { return nearPlane; }
	void SetNearPlane(float nearPl) {
		nearPlane = nearPl;
		BuildProjectionMatrix();
	}

	float GetFarPlane() const { return farPlane; }
	void SetFarPlane(float farPl) {
		farPlane = farPl;
		BuildProjectionMatrix();
	}

	float GetFOV() const { return fov; }
	void SetFOV(float fOV) {
		fov = fOV;
		BuildProjectionMatrix();
	}

	int GetWindowWidth() const { return windowWidth; }
	int GetWindowHeight() const { return windowHeight; }

protected:
	//Builds a view matrix for the current camera variables, suitable for sending straight
	//to a vertex shader (i.e it's already an 'inverse camera matrix').
	void BuildViewMatrix();
	void BuildProjectionMatrix();

	float	yaw;
	float	pitch;
	float roll;
	Vector3 position;
	float speed;
	float nearPlane;
	float farPlane;
	float fov;

	int windowWidth;
	int windowHeight;

	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};