#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"
int main(char* c, int args[]) {
	Window w("My First OpenGL 3 Triangle!", 1280, 720, false);

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}