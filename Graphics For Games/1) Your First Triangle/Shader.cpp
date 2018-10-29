#include "Shader.h"



Shader::Shader(string vertexFile, string fragmentFile, string geometryFile)
{
	program = glCreateProgram();
	objects[SHADER_VERTEX] = GenerateShader(vertexFile, GL_VERTEX_SHADER);
	objects[SHADER_FRAGMENT] = GenerateShader(fragmentFile, GL_FRAGMENT_SHADER);
	objects[SHADER_GEOMETRY] = 0;

	if (!geometryFile.empty()) {
		objects[SHADER_GEOMETRY] = GenerateShader(geometryFile, GL_GEOMETRY_SHADER);

		glAttachShader(program, objects[SHADER_GEOMETRY]);
	}

	glAttachShader(program, objects[SHADER_VERTEX]);
	glAttachShader(program, objects[SHADER_FRAGMENT]);

	SetDefaultAttributes();
}

Shader::~Shader()
{
}
