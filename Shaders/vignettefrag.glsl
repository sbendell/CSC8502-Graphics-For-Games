#version 150 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main (void) {
	float distFromCenter = sqrt(pow((IN.texCoord.x - 0.5), 2) + pow((IN.texCoord.y - 0.5), 2));
	vec4 diffuse = texture2D(diffuseTex, IN.texCoord);
	
	if (distFromCenter > 0.4){
		float factor = (distFromCenter - 0.4) * 2.0;
		
		diffuse = diffuse * (1 - factor);
	}
	fragColour = diffuse;
}