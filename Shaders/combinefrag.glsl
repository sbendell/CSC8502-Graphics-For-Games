#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D emissiveTex;
uniform sampler2D lightSpecularTex;
uniform sampler2D specularTex;

uniform vec3 ambientColour;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main (void) {
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 depth = texture(depthTex, IN.texCoord).xyz;
	vec4 normal = texture(normTex, IN.texCoord);
	vec3 specular = texture(specularTex, IN.texCoord).xyz;
	vec3 light = texture(emissiveTex, IN.texCoord).xyz;
	vec3 lightSpecular = texture(lightSpecularTex, IN.texCoord).xyz;

	vec3 ambient = ambientColour;
	
	if (normal.w == 0.0f) { //(normal.x == 1.0f && normal.y == 1.0f && normal.z == 1.0f){
		ambient = vec3(1.0f, 1.0f, 1.0f);
	}
	fragColour.xyz = diffuse * ambient; // ambient
	fragColour.xyz += (diffuse * light);// lambert
	fragColour.xyz += lightSpecular * specular; // Specular
	fragColour.a = 1.0;
}
