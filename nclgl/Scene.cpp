#include "Scene.h"

Scene::Scene(Renderer* rend, int width, int height, OBJMesh* sphere) {
	renderer = rend;
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z / 2.0f),
		1.0f, 1.0f, 10000.0f, width, height, 45.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	root = new SceneNode();

	SceneNode* terrain = new SceneNode(heightMap);
	terrain->SetMaterial(renderer->GetMaterialWithName("Rocky Terrain"));
	terrain->SetBoundingRadius(100000.0f);
	root->AddChild(terrain);

	float xPos = (RAW_WIDTH * HEIGHTMAP_X);
	float zPos = (RAW_HEIGHT * HEIGHTMAP_Z);

	lights = new Light[LIGHTNUMS1 * LIGHTNUMS1];
	shadowTex = new GLuint[LIGHTNUMS1 * LIGHTNUMS1];

	for (int x = 0; x < LIGHTNUMS1; x++)
	{
		for (int z = 0; z < LIGHTNUMS1; z++)
		{
			lights[x * LIGHTNUMS1 + z] = Light(Vector3(xPos * x, 1000.0f, zPos * z), Vector4(0.2f, 0.2f, 0.2f, 1.0f),
				4000.0f, 15.0f);
		}
	}

	ambientColour = Vector3(0.2f, 0.2f, 0.2f);

	shadowMatrixes = new Matrix4[LIGHTNUMS1 * LIGHTNUMS1];
	GenBuffers();
	pointLightShader = renderer->GetShaderWithName("Point Light");
	shadowShader = renderer->GetShaderWithName("Shadow");
	combineShader = renderer->GetShaderWithName("Combine");
	skyboxShader = renderer->GetShaderWithName("Skybox");
	postProcessShader = renderer->GetShaderWithName("Blur Post Process");
	presentShader = renderer->GetShaderWithName("Texture");
	lightSphere = sphere;
}

Scene::Scene()
{

}


Scene::~Scene()
{
	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &bufferSpecularTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(2, postProcessTex);
	glDeleteTextures(6, shadowTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &postProcessFBO);
	glDeleteFramebuffers(1, &shadowFBO);
}

void Scene::GenBuffers() {
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);
	glGenFramebuffers(1, &postProcessFBO);
	glGenFramebuffers(1, &shadowFBO);

	GLenum GBuffer[5];
	GBuffer[0] = GL_COLOR_ATTACHMENT0;
	GBuffer[1] = GL_COLOR_ATTACHMENT1;
	GBuffer[2] = GL_COLOR_ATTACHMENT2;
	GBuffer[3] = GL_COLOR_ATTACHMENT3;
	GBuffer[4] = GL_COLOR_ATTACHMENT4;

	GLenum lightBuffer[2];
	lightBuffer[0] = GL_COLOR_ATTACHMENT0;
	lightBuffer[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	renderer->GenerateScreenTexture(bufferDepthTex, true);
	renderer->GenerateScreenTexture(bufferColourTex);
	renderer->GenerateScreenTexture(bufferNormalTex);
	renderer->GenerateScreenTexture(bufferSpecularTex);	

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		GL_TEXTURE_2D, bufferSpecularTex, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(3, GBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	renderer->GenerateScreenTexture(lightEmissiveTex);
	renderer->GenerateScreenTexture(lightSpecularTex);

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, lightBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glGenTextures(LIGHTNUMS1 * LIGHTNUMS1, shadowTex);
	for (int i = 0; i < LIGHTNUMS1 * LIGHTNUMS1; i++)
	{
		glBindTexture(GL_TEXTURE_2D, shadowTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
			GL_COMPARE_R_TO_TEXTURE);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex[0], 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderer->GenerateScreenTexture(postProcessTex[0]);
	renderer->GenerateScreenTexture(postProcessTex[1]);

	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, postProcessTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	frameFrustum.FromMatrix(camera->GetProjectionMatrix()*camera->GetViewMatrix());
	root->Update(msec);
}

void Scene::RenderScene(Mesh* screen, Mesh* fullScreen) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawShadowScene();
	DrawSkybox(fullScreen);
	FillBuffers();
	DrawPointLights();
	CombineBuffers(fullScreen);
	PostProcess(fullScreen);
	PresentScene(screen);
}

void Scene::BuildNodeLists(SceneNode * from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetTransform().GetWorldMatrix().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetMaterial()) {
			if (from->GetMaterial()->GetColour().w < 1.0f) {
				transparentNodeList.push_back(from);
			}
			else {
				nodeList.push_back(from);
			}
		}
	}
	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Scene::SortNodeLists() {
	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Scene::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Scene::DrawNodes(bool shadow) {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i), shadow);
	}
	glDisable(GL_CULL_FACE);
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i), shadow);
	}
	glEnable(GL_CULL_FACE);
}

void Scene::DrawNode(SceneNode* n, bool shadow) {
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	if (n->GetMesh()) {
		GLuint shaderProgram = n->GetMaterial()->GetShader()->GetProgram();
		if (!shadow) {
			glUseProgram(shaderProgram);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram,
				"modelMatrix"), 1, false, (float*)&n->GetTransform().GetWorldMatrix());
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"),
				1, false, (float*)&camera->GetViewMatrix());
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projMatrix"),
				1, false, (float*)&camera->GetProjectionMatrix());
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "textureMatrix"),
				1, false, (float*)&n->GetMaterial()->GetTextureMatrix());

			glUniform3fv(glGetUniformLocation(shaderProgram,
				"cameraPos"), 1, (float *)& camera->GetPosition());

			glUniform1i(glGetUniformLocation(shaderProgram,
				"cubeTex"), 16);
		}
		else {
			glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(),
				"modelMatrix"), 1, false, (float*)&n->GetTransform().GetWorldMatrix());
		}

		n->Draw(shadow);
	}
	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Scene::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glUseProgram(shadowShader->GetProgram());

	for (int i = 0; i < LIGHTNUMS1 * LIGHTNUMS1; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, shadowTex[i], 0);

		Matrix4 tempViewMatrix = Matrix4::BuildViewMatrix(lights[i].GetPosition(),
			Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 0, RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));
		shadowMatrixes[i] = biasMatrix * (camera->GetProjectionMatrix() * tempViewMatrix);

		Matrix4 identity;
		identity.ToIdentity();

		glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "viewMatrix"),
			1, false, (float*)&tempViewMatrix);
		glUniformMatrix4fv(glGetUniformLocation(shadowShader->GetProgram(), "projMatrix"),
			1, false, (float*)&camera->GetProjectionMatrix());

		BuildNodeLists(root);
		SortNodeLists();

		glClear(GL_DEPTH_BUFFER_BIT);

		DrawNodes(true);
		ClearNodeLists();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, camera->GetWindowWidth(), camera->GetWindowHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void Scene::DrawSkybox(Mesh* screen) {
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GLuint skyboxProgram = skyboxShader->GetProgram();
	glUseProgram(skyboxProgram);

	glUniform3fv(glGetUniformLocation(skyboxProgram,
		"cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(skyboxProgram,
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

	Matrix4 identity;
	identity.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram,
		"modelMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram,
		"viewMatrix"), 1, false, (float*)&camera->GetViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram,
		"projMatrix"), 1, false, (float*)&camera->GetProjectionMatrix());
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram,
		"textureMatrix"), 1, false, (float*)&identity);
	screen->Draw();

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void Scene::FillBuffers() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT);

	DrawNodes(false);
	ClearNodeLists();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::DrawPointLights() {
	glUseProgram(pointLightShader->GetProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);

	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(pointLightShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glUniform2f(glGetUniformLocation(pointLightShader->GetProgram(), "pixelSize"),
		1.0f / camera->GetWindowWidth(), 1.0f / camera->GetWindowHeight());

	for (int x = 0; x < LIGHTNUMS1; ++x) {
		for (int z = 0; z < LIGHTNUMS1; ++z) {
			Light & l = lights[(x * LIGHTNUMS1) + z];
			float radius = l.GetRadius();

			Matrix4 tempModelMatrix =
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(tempModelMatrix.GetPositionVector());

			glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightPos"), 1, (float*)&l.GetPosition());
			glUniform4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightColour"), 1, (float*)&l.GetColour());
			glUniform1f(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightRadius"), l.GetRadius());
			glUniform1f(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightBrightness"), l.GetBrightness());

			Matrix4 identity;
			identity.ToIdentity();

			glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"modelMatrix"), 1, false, (float*)&tempModelMatrix);
			glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"viewMatrix"), 1, false, (float*)&camera->GetViewMatrix());
			glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"projMatrix"), 1, false, (float*)&camera->GetProjectionMatrix());
			glUniformMatrix4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"textureMatrix"), 1, false, (float*)&identity);

			float dist = (l.GetPosition() - camera->GetPosition()).Length();
			if (dist < radius) {// camera is inside the light volume !
				glCullFace(GL_FRONT);
			}
			else {
				glCullFace(GL_BACK);
			}

			lightSphere->Draw();
		}
	}
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Scene::CombineBuffers(Mesh* screen) {
	glUseProgram(combineShader->GetProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, postProcessTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);

	Matrix4 tempProjMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	
	Matrix4 identity;
	identity.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(combineShader->GetProgram(),
		"modelMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(combineShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)&camera->GetViewMatrix());
	glUniformMatrix4fv(glGetUniformLocation(combineShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&tempProjMatrix);
	glUniformMatrix4fv(glGetUniformLocation(combineShader->GetProgram(),
		"textureMatrix"), 1, false, (float*)&identity);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 2);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "normTex"), 4);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularTex"), 5);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "emissiveTex"), 6);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "lightSpecularTex"), 7);


	glUniform3f(glGetUniformLocation(combineShader->GetProgram(),
		"ambientColour"), ambientColour.x, ambientColour.y, ambientColour.z);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, bufferSpecularTex);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);


	screen->Draw();

	glUseProgram(0);
}

void Scene::PostProcess(Mesh* screen) {
	glUseProgram(postProcessShader->GetProgram());
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, postProcessTex[0], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	Matrix4 tempProjMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	Matrix4 identity;
	identity.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(postProcessShader->GetProgram(),
		"modelMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(postProcessShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(postProcessShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&tempProjMatrix);
	glUniformMatrix4fv(glGetUniformLocation(postProcessShader->GetProgram(),
		"textureMatrix"), 1, false, (float*)&identity);

	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "diffuseTex"), 0);
	glUniform2f(glGetUniformLocation(postProcessShader->GetProgram(),
		"pixelSize"), 1.0f / camera->GetWindowWidth(), 1.0f / camera->GetWindowHeight());
	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(),
		"isVertical"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessTex[1]);

	screen->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Scene::PresentScene(Mesh* screen) {
	glUseProgram(presentShader->GetProgram());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Matrix4 tempProjMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);

	Matrix4 identity;
	identity.ToIdentity();

	glUniformMatrix4fv(glGetUniformLocation(presentShader->GetProgram(),
		"modelMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(presentShader->GetProgram(),
		"viewMatrix"), 1, false, (float*)&identity);
	glUniformMatrix4fv(glGetUniformLocation(presentShader->GetProgram(),
		"projMatrix"), 1, false, (float*)&tempProjMatrix);
	glUniformMatrix4fv(glGetUniformLocation(presentShader->GetProgram(),
		"textureMatrix"), 1, false, (float*)&identity);
	glUniform1i(glGetUniformLocation(postProcessShader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessTex[0]);

	screen->Draw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}