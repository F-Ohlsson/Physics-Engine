#version 430
layout(location=0) in vec4 color;
layout(location=1) in vec2 TexCoord;
out vec4 Colour;
uniform sampler2D tex;
uniform vec4 colour = vec4(0,0,0,0);
uniform float scale = 0.5;
void main()
{
	//Color = texture(tex, TexCoord);
	//Color = color;
	if(colour == (0,0,0,0))
		Colour = vec4(scale,scale,scale,1);
	else
		Colour = vec4(scale,scale,scale,1) * colour;

};