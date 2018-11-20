#include "Scene.h"

Scene::Scene(Renderer* rend, int width, int height, OBJMesh* sphere) {
	renderer = rend;
	camera = new Camera(0.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_WIDTH * HEIGHTMAP_X),
		1.0f, 1.0f, 10000.0f, width, height, 45.0f);

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");

	root = new SceneNode();

	SceneNode* terrain = new SceneNode(heightMap);
	terrain->SetMaterial(renderer->GetMaterialWithName("Rocky Terrain"));
	terrain->SetBoundingRadius(100000.0f);
	root->AddChild(terrain);

	lights = new Light[8 * 8];
	for (int x = 0; x < 8; ++x) {
		for (int z = 0; z < 8; ++z) {
			Light & l = lights[(x * 8) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (8 - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (8 - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / 8);
			l.SetRadius(radius);
		}
	}
	GenBuffers();
	pointLightShader = renderer->GetShaderWithName("Point Light");
	combineShader = renderer->GetShaderWithName("Combine");
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
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
}

void Scene::GenBuffers() {
	glGenFramebuffers(1, &bufferFBO);
	glGenFramebuffers(1, &pointLightFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;

	// Generate our scene depth texture ...
	renderer->GenerateScreenTexture(bufferDepthTex, true);
	renderer->GenerateScreenTexture(bufferColourTex);
	renderer->GenerateScreenTexture(bufferNormalTex);
	renderer->GenerateScreenTexture(lightEmissiveTex);
	renderer->GenerateScreenTexture(lightSpecularTex);

	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

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

void Scene::RenderScene(Mesh* screen) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	FillBuffers();
	DrawPointLights();
	CombineBuffers(screen);
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

void Scene::DrawNodes() {
	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
	glDisable(GL_CULL_FACE);
	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
	glEnable(GL_CULL_FACE);
}

void Scene::DrawNode(SceneNode* n) {
	if (n->GetMesh()) {
		GLuint shaderProgram = n->GetMaterial()->GetShader()->GetProgram();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram,
			"modelMatrix"), 1, false, (float*)&n->GetTransform().GetWorldMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"),
			1, false, (float*)&camera->GetViewMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projMatrix"),
			1, false, (float*)&camera->GetProjectionMatrix());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "textureMatrix"),
			1, false, (float*)&n->GetMaterial()->GetTextureMatrix());

		n->Draw();
	}
}

void Scene::FillBuffers() {
	BuildNodeLists(root);
	SortNodeLists();

	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawNodes();
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

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < 8; ++x) {
		for (int z = 0; z < 8; ++z) {
			Light & l = lights[(x * 8) + z];
			float radius = l.GetRadius();

			Matrix4 tempModelMatrix =
				pushMatrix *
				Matrix4::Rotation(0.0f, Vector3(0, 1, 0)) *
				popMatrix *
				Matrix4::Translation(l.GetPosition()) *
				Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(tempModelMatrix.GetPositionVector());

			glUniform3fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightPos"), 1, (float*)&l.GetPosition());
			glUniform4fv(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightColour"), 1, (float*)&l.GetColour());
			glUniform1f(glGetUniformLocation(pointLightShader->GetProgram(),
				"lightRadius"), l.GetRadius());

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
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "emissiveTex"), 3);
	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularTex"), 4);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	screen->Draw();

	glUseProgram(0);
}