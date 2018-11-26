#version 150 core

uniform sampler2D diffuseTex; // Diffuse texture map
uniform sampler2D bumpTex; // Bump map
uniform sampler2D specularTex;

uniform sampler2D rdiffuseTex; // Diffuse texture map
uniform sampler2D rbumpTex; // Bump map
uniform sampler2D rspecularTex;

uniform sampler2D sdiffuseTex; // Diffuse texture map
uniform sampler2D sbumpTex; // Bump map
uniform sampler2D sspecularTex;

uniform sampler2D ldiffuseTex; // Diffuse texture map
uniform sampler2D lbumpTex; // Bump map
uniform sampler2D lspecularTex;

uniform sampler2D craterTex;

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

	vec3 gnormal = normalize(TBN * normalize((texture2D(bumpTex, IN.texCoord ).rgb ) * 2.0 - 1.0));
	vec4 gcolour = texture2D(diffuseTex, IN.texCoord);
	vec4 gspecular = texture2D(specularTex, IN.texCoord);

	vec3 snormal = normalize(TBN * normalize((texture2D(sbumpTex, IN.texCoord ).rgb ) * 2.0 - 1.0));
	vec4 scolour = texture2D(sdiffuseTex, IN.texCoord);
	vec4 sspecular = texture2D(sspecularTex, IN.texCoord);
	
	vec3 finalNormal = mix(gnormal, snormal, IN.worldPos.y / 300);
	vec4 finalColour = mix(gcolour, scolour, IN.worldPos.y / 300);
	vec4 finalSpecular = mix(gspecular, sspecular, IN.worldPos.y / 300);
	
	finalNormal = mix(texture2D(lbumpTex, IN.texCoord).rgb, finalNormal, texture2D(craterTex, IN.texCoord / 16).r); 
	finalColour = mix(texture2D(ldiffuseTex, IN.texCoord), finalColour, texture2D(craterTex, IN.texCoord / 16).r); 
	finalSpecular = mix(texture2D(lspecularTex, IN.texCoord), finalSpecular, texture2D(craterTex, IN.texCoord / 16).r ); 

	finalColour = mix(texture2D(rdiffuseTex, IN.texCoord), finalColour, (IN.normal.y - 0.7) * 3);
	finalNormal = normalize(mix(texture2D(rbumpTex, IN.texCoord).xyz, finalNormal, (IN.normal.y - 0.7) * 3));
	finalSpecular = mix(texture2D(rspecularTex, IN.texCoord), finalSpecular, (IN.normal.y - 0.7) * 3);
	
	fragColour [0] = finalColour;
	fragColour [1] = vec4(finalNormal, 1.0);
	fragColour [2] = finalSpecular;
}