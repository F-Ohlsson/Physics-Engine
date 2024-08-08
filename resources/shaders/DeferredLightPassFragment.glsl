#version 430
in vec2 TexCoordinates;

uniform vec3 camPos;

//Textures from framebuffer
uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

//Directional light uniforms
uniform vec3 dirLightDirection;
uniform float dirLightIntensity;
uniform vec4 dirLightColour;

//Material uniforms
uniform float shininess;
uniform vec4 materialAmbientColour;
uniform vec4 materialDiffuseColour;
uniform vec4 materialSpecularColour;


struct PointLight{
	vec3 pos;
	vec4 colour;
	float intensity;
};
////Point Light uniforms
//const int maxLights = 30;
//uniform int numLights;
//uniform PointLight pointLights[maxLights];

PointLight pointLight;


out vec4 Result;

void main()
{
	vec3 fragPos = texture(gPos, TexCoordinates).rgb;
	vec3 normal = texture(gNormal, TexCoordinates).rgb;
	vec3 albedo = texture(gAlbedo, TexCoordinates).rgb;

	
	vec3 viewDirection;
	vec3 halfDirection;
	float specularAngle;

//	vec3 test = pointLights[0].pos;
//	if(test.x >0)
//		discard;
//
//	if(camPos.x < 0)
//		discard;
//	if(numLights == 25)
//		discard;

	//Begin Directional Light work
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
//	End Directional Light work
	vec4 pointDiffTerm = vec4(0,0,0,0);
	vec4 pointSpecTerm = vec4(0,0,0,0);

	vec3 pointLightDirection = pointLight.pos - fragPos;
//
//		if(pointLightDirection.x > 0)
//			discard;
//

	float pointLightDistance = length(pointLightDirection);

	if(pointLightDistance > 1)
		discard;

	pointLightDistance = pow(pointLightDistance,2);
	pointLightDirection = normalize(pointLightDirection); //L in slides

	float pointDiffuseIrradiance = max(dot(pointLightDirection,normal),0.0); //This is giving me issues

	//Specular
	float pointSpecular = 0.0;
	if(pointDiffuseIrradiance > 0.0){
		viewDirection = normalize(camPos-fragPos);
		halfDirection = normalize(pointLightDirection + viewDirection); 
		specularAngle = max(dot(halfDirection, normal),0.0);
		pointSpecular = pow(specularAngle, shininess); 
	}
	pointDiffTerm += materialDiffuseColour * pointLight.colour*pointDiffuseIrradiance*(pointLight.intensity/pointLightDistance);
	pointSpecTerm += materialSpecularColour * pointSpecular * pointLight.intensity/pointLightDistance;
	//End Point Light work

	vec4 ambientTerm = materialAmbientColour * materialDiffuseColour;

	vec4 dirDiffTerm = dirLightColour * dirDiffuseIrradiance * dirLightIntensity;
	//vec4 pointDiffTerm = materialDiffuseColour * pointLightColour*pointDiffuseIrradiance*(pointLightIntensity/pointLightDistance); //intensity/distance makes light dimmer the further it travels
	vec4 diffuseTerm = dirDiffTerm + pointDiffTerm;
	
	vec4 dirSpecTerm = dirLightColour * dirSpecular * dirLightIntensity;
////	vec4 pointSpecTerm = materialSpecularColour * pointSpecular * pointLightIntensity/pointLightDistance; //intensity/distance makes light dimmer the further it travels
	vec4 specularTerm = dirSpecTerm+pointSpecTerm;
//
//
//
	Result = vec4(albedo,0) * (ambientTerm + diffuseTerm + specularTerm); //Final result

	//Result = vec4(1,0,0,1);
};