#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec3 normal;

layout(location=0) out vec3 Pos; //Send this through here so it's interpolated
layout(location=1) out vec2 TexCoord;
layout(location=2) out vec3 Normal;
layout(location=3) out vec3 WorldPos;

uniform mat4 mvp;//modelviewprojection
uniform mat4 m; //model matrix
uniform mat4 mTransposedInverse; //normal matrix

void main()
{
	gl_Position =  mvp * vec4(pos, 1);

	vec4 worldPos = m * vec4(pos,1); //Vertex world position
	WorldPos = vec3(worldPos);
	TexCoord = texCoord;
	Normal = normalize(vec3(mTransposedInverse * vec4(normal,0.0)));
};