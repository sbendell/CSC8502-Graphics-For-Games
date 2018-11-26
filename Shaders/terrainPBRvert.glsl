#version 150 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform sampler2D craterTex;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent; // New !
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent; // New !
	vec3 binormal; // New !
	vec3 worldPos;
} OUT;

void main ( void ) {
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	float height = texture2D(craterTex, texCoord.xy / 16.0f).r;

	OUT.colour = colour ;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = normalize(normalMatrix * normalize(tangent));
	OUT.binormal = normalize(normalMatrix * normalize(cross(normal, tangent)));

	OUT.worldPos = (modelMatrix * vec4(position.x, position.y * height, position.z, 1)).xyz;

	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position.x, position.y * height, position.z, 1);
}
