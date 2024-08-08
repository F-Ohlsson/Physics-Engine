#version 430

layout(location = 0) in vec4 NDC;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightIntensity;

uniform vec3 camPos;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

//Material uniforms
uniform float shininess;
uniform vec4 materialAmbientColour;
uniform vec4 materialDiffuseColour;
uniform vec4 materialSpecularColour;

out vec4 Result;

void main()
{
	vec2 texCoordinates = ((NDC.xy/NDC.w)+1.0f)*0.5f;//moves values from [-1,1] to [0,1]

	vec3 fragPos = texture(gPos, texCoordinates).rgb;
	vec3 normal = texture(gNormal, texCoordinates).rgb;
	vec3 albedo = texture(gAlbedo, texCoordinates).rgb;

	vec3 viewDirection;
	vec3 halfDirection;
	float specularAngle;

	vec4 pointDiffTerm = vec4(0,0,0,0);
	vec4 pointSpecTerm = vec4(0,0,0,0);

	vec3 pointLightDirection = lightPos - fragPos;

	float pointLightDistance = length(pointLightDirection);

	if(pointLightDistance <= lightRadius){ //Only do light calculations if distance between light and fragment is less than the radius of the light
	
		pointLightDistance = pow(pointLightDistance,2);
		pointLightDirection = normalize(pointLightDirection);

		float pointDiffuseIrradiance = max(dot(pointLightDirection,normal),0.0);

		//Specular
		float pointSpecular = 0.0;
		if(pointDiffuseIrradiance > 0.0){
			viewDirection = normalize(camPos-fragPos);
			halfDirection = normalize(pointLightDirection + viewDirection); 
			specularAngle = max(dot(halfDirection, normal),0.0);
			pointSpecular = pow(specularAngle, shininess); 
		}
		pointDiffTerm = materialDiffuseColour * lightColour*pointDiffuseIrradiance*(lightIntensity/pointLightDistance);
		pointSpecTerm = lightColour * pointSpecular * lightIntensity/pointLightDistance;

	}

	Result = vec4(pointDiffTerm + pointSpecTerm); //Final result
};