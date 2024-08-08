#version 430
layout(location=0) in vec3 Pos;
layout(location=1) in vec2 TexCoord;
layout(location=2) in vec3 Normal;
layout(location=3) in vec3 WorldPos;

out vec4 Result;

uniform sampler2D tex;
uniform vec3 camPos;

//Light uniforms
uniform vec3 pointLightPosition;
uniform float pointLightIntensity;
uniform vec4 pointLightColour;

uniform vec3 dirLightDirection;
uniform float dirLightIntensity;
uniform vec4 dirLightColour;

//Material uniforms
uniform float shininess;
uniform vec4 materialAmbientColour;
uniform vec4 materialDiffuseColour;
uniform vec4 materialSpecularColour;

void main()
{
	vec3 viewDirection;//V in slides. 
	vec3 halfDirection; //H in slides
	float specularAngle;

	//Begin Directional Light work
	//Diffuse
	vec3 normDirLightDir = normalize(dirLightDirection);
	float dirDiffuseIrradiance = max(dot(normDirLightDir, Normal),0.0); //Clamp irradiance between 0 and 1

	//Specular
	float dirSpecular = 0.0;
	if(dirDiffuseIrradiance > 0.0){
		viewDirection = normalize(camPos - WorldPos);
		halfDirection = normalize(normDirLightDir + viewDirection);
		specularAngle = max(dot(halfDirection, Normal),0.0);
		dirSpecular = pow(specularAngle, shininess);
	}
	//End Directional Light work

	//Begin Point Light work
	vec3 pointLightDirection = pointLightPosition - WorldPos;

	float pointLightDistance = length(pointLightDirection);

	pointLightDistance = pow(pointLightDistance,2);
	pointLightDirection = normalize(pointLightDirection); //L in slides

	float pointDiffuseIrradiance = max(dot(pointLightDirection,Normal),0.0);

	//Specular
	float pointSpecular = 0.0;
	if(pointDiffuseIrradiance > 0.0){
		viewDirection = normalize(camPos-WorldPos);
		halfDirection = normalize(pointLightDirection + viewDirection); 
		specularAngle = max(dot(halfDirection, Normal),0.0);
		pointSpecular = pow(specularAngle, shininess); 
	}
	//End Point Light work

	vec4 ambientTerm = materialAmbientColour * materialDiffuseColour;
	
	vec4 dirDiffTerm = dirLightColour * dirDiffuseIrradiance * dirLightIntensity;
	vec4 pointDiffTerm = materialDiffuseColour * pointLightColour*pointDiffuseIrradiance*(pointLightIntensity/pointLightDistance); //intensity/distance makes light dimmer the further it travels
	vec4 diffuseTerm = dirDiffTerm + pointDiffTerm;
	
	vec4 dirSpecTerm = dirLightColour * dirSpecular * dirLightIntensity;
	vec4 pointSpecTerm = materialSpecularColour * pointSpecular * pointLightIntensity/pointLightDistance; //intensity/distance makes light dimmer the further it travels
	vec4 specularTerm = dirSpecTerm+pointSpecTerm;

	//Result = texture(tex, TexCoord) * (ambientTerm + pointDiffTerm + pointSpecTerm); //Only Point Light
	//Result = texture(tex, TexCoord) * (ambientTerm + dirDiffTerm + dirSpecTerm); //Only Directional Light
	Result = texture(tex, TexCoord) * (ambientTerm + diffuseTerm + specularTerm);
	//Result = vec4(Normal,1.0); //To check out normals
};