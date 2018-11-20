#version 150 core

uniform samplerCube cubeTex;
uniform vec3 cameraPos;

in Vertex {
	vec3 normal;
} IN;

out vec4 fragColour[2];

void main (void) {
	fragColour [0] = texture(cubeTex, normalize(IN.normal));
	fragColour [1] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}