#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec2 textureCoords;

out vec2 TexCoordinates;

void main()
{
	gl_Position =  vec4(pos.x, pos.y, 0.0,1.0);
	TexCoordinates = textureCoords;	
	//TexCoordinates = vec2(4,4);
};