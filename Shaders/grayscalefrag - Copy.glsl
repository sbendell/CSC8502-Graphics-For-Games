#version 150 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main (void) {
	vec4 colour = texture2D(diffuseTex, IN.texCoord);
	
	float sumCol = ((pow(colour.r, 2.2) * 0.2126) + (pow(colour.g, 2.2) * 0.7152) + (pow(colour.b, 2.2) * 0.0722)) / 3;
	float finalCol = 50 * pow(sumCol, 0.33) - 16;
	
	fragColour = vec4(finalCol, finalCol, finalCol, 1.0);
}