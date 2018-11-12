#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform sampler2D diffuseTex;
uniform sampler2D craterTex;

in vec3 position;
in vec2 texCoord;
in vec4 colour;

out Vertex {
	vec2 texCoord;
	vec4 colour;
} OUT;

void main (void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	float height = texture2D(craterTex, texCoord.xy / 16.0f).r;
	gl_Position = mvp * vec4 (position.x, position.y * height, position.z, 1.0);
	OUT.texCoord = (textureMatrix * vec4 (texCoord, 0.0, 1.0)).xy;
	OUT.colour = colour;
}