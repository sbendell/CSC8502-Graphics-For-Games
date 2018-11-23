#version 150 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main (void) {
	vec4 colour = texture2D(diffuseTex, IN.texCoord);
	
	float sumCol = (colour.r + colour.b + colour.g) / 3;
	
	fragColour = vec4(sumCol, sumCol, sumCol, 1.0);
}