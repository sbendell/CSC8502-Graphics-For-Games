#include "Mesh.h"

Mesh::Mesh(void)
{
	for (int i = 0; i < MAX_BUFFER; i++)
	{
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;
	texture = 0;
	textureCoords = NULL;
}


Mesh::~Mesh(void)
{
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	delete[] vertices;
	delete[] colours;
	delete[] textureCoords;
}

Mesh* Mesh::GenerateTriangle() {
	Mesh* m = new Mesh();
	m->numVertices = 3;
	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 1.0f, 0.0f);
	m->vertices[1] = Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[2] = Vector3(-1.0f, -1.0f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateSquare() {
	Mesh* m = new Mesh();
	m->numVertices = 6;
	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-0.5f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, 0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);
	m->vertices[3] = Vector3(0.5f, 0.5f, 0.0f);
	m->vertices[4] = Vector3(-0.5f, -0.5f, 0.0f);
	m->vertices[5] = Vector3(0.5f, -0.5f, 0.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[3] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[4] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	m->colours[5] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh* Mesh::GenerateCube() {
	Mesh* m = new Mesh();
	m->numVertices = 14;
	m->type = GL_TRIANGLE_STRIP;
	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(-1.f, 1.f, 1.f);
	m->vertices[1] = Vector3(1.f, 1.f, 1.f);
	m->vertices[2] = Vector3(-1.f, -1.f, 1.f);
	m->vertices[3] = Vector3(1.f, -1.f, 1.f);
	m->vertices[4] = Vector3(1.f, -1.f, -1.f);
	m->vertices[5] = Vector3(1.f, 1.f, 1.f);
	m->vertices[6] = Vector3(1.f, 1.f, -1.f);
	m->vertices[7] = Vector3(-1.f, 1.f, 1.f);
	m->vertices[8] = Vector3(-1.f, 1.f, -1.f);
	m->vertices[9] = Vector3(-1.f, -1.f, 1.f);
	m->vertices[10] = Vector3(-1.f, -1.f, -1.f);
	m->vertices[11] = Vector3(1.f, -1.f, -1.f);
	m->vertices[12] = Vector3(-1.f, 1.f, -1.f);
	m->vertices[13] = Vector3(1.f, 1.f, -1.f);

	m->colours = new Vector4[m->numVertices];
	for (int i = 0; i < 14; i++)
	{
		m->colours[i] = Vector4(0.06f * i, 0.0f, 0.0f, 1.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::BufferData() {
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);
	if (colours) {
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	glBindVertexArray(0);
}

void Mesh::Draw() {
	glBindVertexArray(arrayObject);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(type, 0, numVertices);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}