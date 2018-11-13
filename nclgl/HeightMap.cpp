#include "HeightMap.h"

HeightMap::HeightMap(std::string name) {
	std::ifstream file(name.c_str(), ios::binary);
	if (!file) {
		return;
	}
	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	colours = new Vector4[numVertices];
	indices = new GLuint[numIndices];

	unsigned char * data = new unsigned char[numVertices];
	file.read((char *)data, numVertices * sizeof(unsigned char));
	file.close();

	for (int x = 0; x < RAW_WIDTH; ++x) {
		for (int z = 0; z < RAW_HEIGHT; ++z) {
			int offset = (x * RAW_WIDTH) + z;

			vertices[offset] = Vector3(x * HEIGHTMAP_X, data[offset] * HEIGHTMAP_Y, z * HEIGHTMAP_Z);

			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
			colours[offset] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	delete data;
	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {
			int a = (x * (RAW_WIDTH)) + z;
			int b = ((x + 1) * (RAW_WIDTH)) + z;
			int c = ((x + 1) * (RAW_WIDTH)) + (z + 1);
			int d = (x * (RAW_WIDTH)) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}
	glGenTextures(1, &craterTex);
	pixels = new float[256 * 256 * 4];
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			pixels[4 * (i * 256 + j) + 0] = 1.0f;
			pixels[4 * (i * 256 + j) + 1] = 1.0f;
			pixels[4 * (i * 256 + j) + 2] = 1.0f;
			pixels[4 * (i * 256 + j) + 3] = 1.0f;
		}
	}
	GenerateCraterTexture();
	GenerateNormals();
	GenerateTangents();
	BufferData();
}

void HeightMap::GenerateCraterTexture() {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, craterTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void HeightMap::SmashTerrain(int xPos, int yPos) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, craterTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels);
	for (int i = -20; i < 20; i++)
	{
		for (int j = -20; j < 20; j++)
		{
			float temp = sqrt(pow(i, 2) + pow(j, 2));
			int index = 4 * ((xPos + i) * 256 + (yPos + j)) + 0;
			if (temp < 20) {
				pixels[index + 0] = pixels[index + 0] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 1] = pixels[index + 1] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 2] = pixels[index + 2] * (1.0f - cos((temp / 20.0f) * (PI / 2.0f)));
				pixels[index + 3] = 1.0f;
			}
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_FLOAT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void HeightMap::Draw() {
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(arrayObject);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, craterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bumpTexture);

	if (bufferObject[INDEX_BUFFER]) {
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
