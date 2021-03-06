#include "Camera.h"

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float msec)	{
	//Update the mouse by how much
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw	  -= (Window::GetMouse()->GetRelativePosition().x);

	if (Window::GetMouse()->ButtonDown(MOUSE_LEFT)) {
		fov -= 0.5f;
		BuildProjectionMatrix();
	}
	if (Window::GetMouse()->ButtonDown(MOUSE_RIGHT)) {
		fov += 0.5f;
		BuildProjectionMatrix();
	}

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = min(pitch,75.0f);
	pitch = max(pitch,-75.0f);

	if(yaw <0) {
		yaw+= 360.0f;
	}
	if(yaw > 360.0f) {
		yaw -= 360.0f;
	}

	msec *= speed;

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(0,0,-1) * msec;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position += Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position -= Matrix4::Rotation(yaw, Vector3(0,1,0)) * Vector3(-1,0,0) * msec;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Q)) {
		roll -= 1.0f;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_E)) {
		roll += 1.0f;
	}

	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y -= msec;
	}
	if(Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += msec;
	}

	roll = min(roll, 45.0f);
	roll = max(roll, -45.0f);
	BuildViewMatrix();
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
void Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	viewMatrix =	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(-roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
};

void Camera::BuildProjectionMatrix() {
	projectionMatrix = Matrix4::Perspective(nearPlane, farPlane,
		(float)windowWidth / (float)windowHeight, fov);
}