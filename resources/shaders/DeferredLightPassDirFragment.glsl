#version 430
in vec2 TexCoordinates;

uniform vec3 camPos;

//Textures from framebuffer
uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

uniform sampler2D pointLightResults;


//Directional light uniforms
uniform vec3 dirLightDirection;
uniform float dirLightIntensity;
uniform vec4 dirLightColour;

//Material uniforms
uniform float shininess;
uniform vec4 materialAmbientColour;
uniform vec4 materialDiffuseColour;
uniform vec4 materialSpecularColour;


out vec4 Result;

void main()
{
	vec3 fragPos = texture(gPos, TexCoordinates).rgb;
	vec3 normal = texture(gNormal, TexCoordinates).rgb;
	vec3 albedo = texture(gAlbedo, TexCoordinates).rgb;
	vec3 plResults = texture(pointLightResults, TexCoordinates).rgb;

	
	vec3 viewDirection;
	vec3 halfDirection;
	float specularAngle;

	//Diffuse
	vec3 normDirLightDir = normalize(dirLightDirection);
	float dirDiffuseIrradiance = max(dot(normDirLightDir, normal),0.0); //Clamp irradiance between 0 and 1

	//Specular
	float dirSpecular = 0.0;
	if(dirDiffuseIrradiance > 0.0){
		viewDirection = normalize(camPos - fragPos);
		halfDirection = normalize(normDirLightDir + viewDirection);
		specularAngle = max(dot(halfDirection, normal),0.0);
		dirSpecular = pow(specularAngle, shininess);
	}


	vec4 ambientTerm = materialAmbientColour * materialDiffuseColour;
	vec4 dirDiffTerm = dirLightColour * dirDiffuseIrradiance * dirLightIntensity;
	vec4 dirSpecTerm = dirLightColour * dirSpecular * dirLightIntensity;

	Result = (vec4(albedo,0) + vec4(plResults,0)) * (ambientTerm + dirDiffTerm + dirSpecTerm); //Final result
};