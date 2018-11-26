#version 150 core

uniform sampler2D diffuseTex; // Diffuse texture map
uniform sampler2D bumpTex; // Bump map
uniform sampler2D specularTex;
uniform sampler2D metalnessTex;

uniform samplerCube cubeTex;
uniform vec3 cameraPos;
uniform vec4 tintColour;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour [3]; // Our final outputted colours !

void main (void) {
	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN * normalize((texture2D(bumpTex, IN.texCoord ).rgb ) * 2.0 - 1.0));
	vec4 metalness = texture(metalnessTex, IN.texCoord);

	vec3 incident = normalize(IN.worldPos - cameraPos);
	vec4 reflection = texture(cubeTex, reflect(incident, normalize(IN.normal).xyz));

	
	fragColour [0] = mix(texture2D(diffuseTex , IN.texCoord), vec4((reflection.xyz * metalness.xyz), 1.0), metalness.r) * tintColour;
	fragColour [1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
	fragColour [2] = texture2D(specularTex , IN.texCoord);
}