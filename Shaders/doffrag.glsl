#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform vec2 pixelSize;
uniform int isVertical;
uniform float focus;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;
// Technically , this isn�t quite a �gaussian� distribution ...
const float weights [5] = float [](0.12 , 0.22 , 0.32 , 0.22 , 0.12);

void main (void) {
	vec2 values [5];
	float depth = texture2D(depthTex, IN.texCoord).r;
	float distance = depth - focus;

	if(isVertical == 1) {
		values = vec2[](vec2 (0.0 , -pixelSize.y * 2) ,
		vec2(0.0, -pixelSize.y * 1), vec2(0.0, 0.0) ,
		vec2(0.0, pixelSize.y * 1), vec2(0.0, pixelSize.y * 2) );
	}
	else {
		values = vec2[](vec2(-pixelSize.x * 2, 0.0) ,
		vec2 (-pixelSize.x * 1, 0.0), vec2(0, 0.0),
		vec2 (pixelSize.x * 1, 0.0), vec2(pixelSize.x * 2, 0.0));
	}

	for (int i = 0; i < 5; i ++ ) {
		vec4 tmp = texture2D (diffuseTex, IN.texCoord.xy + values [i]);
		fragColour += tmp * weights[i] * distance;
	}
}
