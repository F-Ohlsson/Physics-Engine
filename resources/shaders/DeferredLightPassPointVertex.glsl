#version 430
layout(location=0) in vec3 pos;

layout(location = 0) out vec4 NDC; //Essentially coordinates corresponding to -1 to 1



uniform mat4 viewProjection;
uniform float lightRadius;
uniform vec3 lightPos;

//For checking in RenderDoc


void main()
{
	vec4 wPos = vec4((pos*lightRadius) + lightPos, 1.0f);
	vec4 ndc = viewProjection * wPos;
	NDC = ndc;
	gl_Position =  ndc; //ndc is 0
};