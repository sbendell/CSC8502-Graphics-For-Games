#version 150

uniform sampler2D diffuseTex;
uniform sampler2D mixTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void){
	vec4 firstCol = texture(diffuseTex, IN.texCoord);
	vec4 secondCol = texture(mixTex, IN.texCoord);
	fragColor = mix(firstCol, secondCol, 0.5) * IN.colour;
}