#version 430
layout(location=0) in vec3 pos;
layout(location=1) in vec2 texCoord;
layout(location=2) in vec3 normal;
layout(location=3) in vec4 tangent;

layout(location=0) out vec3 Pos; //Send this through here so it's interpolated
layout(location=1) out vec2 TexCoord;
layout(location=2) out vec3 WorldPos;
layout(location=3) out mat3 TBN;


uniform mat4 mvp;//modelviewprojection
uniform mat4 m; //model matrix
uniform mat4 mTransposedInverse; //normal matrix

void main()
{
	gl_Position =  mvp * vec4(pos, 1);

	vec4 worldPos = m * vec4(pos,1); //Vertex world position
	WorldPos = vec3(worldPos);
	TexCoord = texCoord;

	vec3 bitangent = cross(tangent.xyz, normal) * tangent.w;
	vec3 Normal = normalize(vec3(mTransposedInverse * vec4(normal,0.0)));
	vec3 Tangent = normalize(vec3(mTransposedInverse * vec4(tangent.xyz, 0)));
	vec3 Bitangent = normalize(vec3(mTransposedInverse * vec4(bitangent,0.0)));
	TBN = mat3(Tangent, Bitangent, Normal);
	//WorldPos = Bitangent.xyz;
};