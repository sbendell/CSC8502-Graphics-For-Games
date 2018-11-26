#version 150 core

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main (void) {
	float distFromCenter = sqrt(pow((IN.texCoord.x - 0.5), 2) + pow((IN.texCoord.y - 0.5), 2));
	
	fragColour = vec4(texture2D(diffuseTex, IN.texCoord + (pixelSize * 10) * distFromCenter).r, 
	texture2D(diffuseTex, IN.texCoord).g,
	texture2D(diffuseTex, IN.texCoord - (pixelSize * 10) * distFromCenter).b,	1.0);
}