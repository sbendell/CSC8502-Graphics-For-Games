#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= new Camera(0, -90.0f, 0, Vector3(-180,60,0), 1);

#ifdef MD5_USE_HARDWARE_SKINNING
	currentShader   = new Shader("skeletonVertexSimple.glsl", SHADERDIR"TexturedFragment.glsl");
#else
	currentShader   = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
#endif

	hellData		= new MD5FileData(MESHDIR"hellknight.md5mesh");

	hellNode		= new MD5Node(*hellData);

	if(!currentShader->LinkProgram()) {
		return;
	}

	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");

	projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void)	{
	delete camera;

	delete hellData;
	delete hellNode;
}

 void Renderer::UpdateScene(float msec)	{
	camera->UpdateCamera(msec); 
	viewMatrix		= camera->BuildViewMatrix();

	hellNode->Update(msec);
	cout << "FPS: " << 1000.0f / msec << endl;
}

void Renderer::RenderScene()	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	UpdateShaderMatrices();
	unsigned int index = 0;
	for (vector<Mesh*>::const_iterator i = ((MD5Mesh*)hellNode->GetMesh())->GetChildMeshIteratorStart();
		i != ((MD5Mesh*)hellNode->GetMesh())->GetChildMeshIteratorEnd(); ++i) {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, (*i)->GetTexture());
		index++;
	}
	glBindTexture(GL_TEXTURE_2D, hellNode->GetMesh()->GetTexture());

	for(int y = 0; y < 5; ++y) {
		for(int z = 0; z < 5; ++z) {
			for (int x = 0; x < 5; ++x) {
				modelMatrix = Matrix4::Translation(Vector3(x * 100, y * 150, z * 100));
				UpdateShaderMatrices();
				hellNode->Draw(*this);
			}
		}
	}

	for (int i = 0; i < index; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
	SwapBuffers();
}