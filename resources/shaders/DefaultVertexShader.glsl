#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec4 color;
layout(location=2) in vec2 texCoord;
layout(location=0) out vec4 Color;
layout(location=1) out vec2 TexCoord;
uniform mat4 mvp;
void main()
{
	gl_Position =  mvp * vec4(pos, 1);
	Color = color;
	TexCoord = texCoord;
};