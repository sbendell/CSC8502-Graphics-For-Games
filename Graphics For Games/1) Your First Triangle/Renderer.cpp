#include "Renderer.h"



Renderer::Renderer(Window& parent):
	OGLRenderer(parent)
{
	triangle = Mesh::GenerateTriangle();

	currentShader = new Shader("../../Shaders/basicVertex.glsl",
		"../../Shader/colourFragment.glsl");

	if (!currentShader->LinkProgram) {
		return;
	}

	init = true;
}


Renderer::~Renderer()
{
	delete triangle;
}
