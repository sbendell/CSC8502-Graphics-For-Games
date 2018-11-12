#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh {
public:
	HeightMap(std::string name);
	~HeightMap(void) {};

	virtual void Draw();

	GLuint GetCraterTex() { return craterTex; }
	void SetCraterTex(GLuint t) { craterTex = t; }

	void SmashTerrain(int xPos, int yPos);

protected:
	GLuint craterTex;
	float* pixels;
};
