#version 430
layout(location=0) in vec2 TexCoord;
layout(location=1) in vec3 WorldPos;
layout(location=2) in mat3 TBN;

layout(location=0) out vec4 Position;
layout(location=1) out vec4 Normal;
layout(location=2) out vec4 Albedo;


uniform sampler2D tex;
uniform sampler2D normalMap;
uniform vec3 camPos;

void main()
{

	//Setup Normals from normal map
	vec3 normal = texture(normalMap, TexCoord.st).xyz;
	normal.g = 1 - normal.g; //Flip Y-axis
	normal = normalize(normal * 2.0-1.0); //"unpack" map so value is from -1,1 again
	normal = TBN*normal;

	//For deferred
	Position = vec4(WorldPos,0);//Position
	Normal = vec4(normal,0.0); //To check out vertex + map normals
	Albedo = texture(tex, TexCoord); //Albedo

	//Visual tests
	//Position = vec4(WorldPos,0);//Position
	//Position = vec4(normal,0.0); //To check out vertex + map normals
	//Position = texture(tex, TexCoord); //Albedo

};