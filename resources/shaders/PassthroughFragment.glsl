#version 430

in vec2 TexCoordinates;

uniform sampler2D tex;

out vec4 Result;

void main()
{
	//Result = vec4(1,0,0,1);
	Result = texture(tex,TexCoordinates);
};