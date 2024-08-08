#version 430
layout(location=0) in vec3 Pos;
layout(location=1) in vec2 TexCoord;
layout(location=2) in vec3 WorldPos;
layout(location=3) in mat3 TBN;

out vec4 Result;

uniform sampler2D tex;
uniform sampler2D normalMap;

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

struct PointLight{
	vec3 pos;
	vec4 colour;
	float intensity;
};	

const int maxLights = 30;
uniform int numLights;
uniform PointLight pointLights[maxLights];


void main()
{

	vec3 viewDirection;
	vec3 halfDirection;
	float specularAngle;

//	vec3 test = pointLights[0].pos;
//	if(test.x >0)
//		discard;

	//Setup Normals from normal map
	vec3 normal = texture(normalMap, TexCoord.st).xyz;
	normal.g = 1 - normal.g; //Flip Y-axis
	normal = normalize(normal * 2.0-1.0); //"unpack" map so value is from -1,1 again
	normal = TBN*normal;

	//Begin Directional Light work
	//Diffuse
	vec3 normDirLightDir = normalize(dirLightDirection);
	float dirDiffuseIrradiance = max(dot(normDirLightDir, normal),0.0); //Clamp irradiance between 0 and 1

	//Specular
	float dirSpecular = 0.0;
	if(dirDiffuseIrradiance > 0.0){
		viewDirection = normalize(camPos - WorldPos);
		halfDirection = normalize(normDirLightDir + viewDirection);
		specularAngle = max(dot(halfDirection, normal),0.0);
		dirSpecular = pow(specularAngle, shininess);
	}
	//End Directional Light work
	
	vec4 pointDiffTerm = vec4(0,0,0,0);
	vec4 pointSpecTerm = vec4(0,0,0,0);
	for(int i = 0; i < numLights; i ++){
			//Begin Point Light work
		vec3 pointLightDirection = pointLights[i].pos - WorldPos;

		float pointLightDistance = length(pointLightDirection);

		pointLightDistance = pow(pointLightDistance,2);
		pointLightDirection = normalize(pointLightDirection); //L in slides

		float pointDiffuseIrradiance = max(dot(pointLightDirection,normal),0.0);

		//Specular
		float pointSpecular = 0.0;
		if(pointDiffuseIrradiance > 0.0){
			viewDirection = normalize(camPos-WorldPos);
			halfDirection = normalize(pointLightDirection + viewDirection); 
			specularAngle = max(dot(halfDirection, normal),0.0);
			pointSpecular = pow(specularAngle, shininess); 
		}
		pointDiffTerm += materialDiffuseColour * pointLights[i].colour*pointDiffuseIrradiance*(pointLights[i].intensity/pointLightDistance);
		pointSpecTerm += materialSpecularColour * pointSpecular * pointLights[i].intensity/pointLightDistance;
	}
	//End Point Light work

	vec4 ambientTerm = materialAmbientColour * materialDiffuseColour;
	
	vec4 dirDiffTerm = dirLightColour * dirDiffuseIrradiance * dirLightIntensity;
//	vec4pointDiffTerm = materialDiffuseColour * pointLightColour*pointDiffuseIrradiance*(pointLightIntensity/pointLightDistance); //intensity/distance makes light dimmer the further it travels
	vec4 diffuseTerm = dirDiffTerm + pointDiffTerm;
	
	vec4 dirSpecTerm = dirLightColour * dirSpecular * dirLightIntensity;
//	vec4 pointSpecTerm = materialSpecularColour * pointSpecular * pointLightIntensity/pointLightDistance; //intensity/distance makes light dimmer the further it travels
	vec4 specularTerm = dirSpecTerm+pointSpecTerm;


	Result = texture(tex, TexCoord) * (ambientTerm + diffuseTerm + specularTerm); //Final result
	//Result = texture(tex, TexCoord) * (ambientTerm + pointDiffTerm + pointSpecTerm); //Only Point Light
	//Result = texture(tex, TexCoord) * (ambientTerm + dirDiffTerm + dirSpecTerm); //Only Directional Light
	//Result = texture(normalMap, TexCoord); //To check out normal map texture

	//For deferred
	//Result = vec4(WorldPos,0);//Position
	//Result = texture(tex, TexCoord); //Albedo
	//Result = vec4(normal,0.0); //To check out vertex + map normals
};