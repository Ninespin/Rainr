#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
uniform mat4 uViewProj;

out VertexData{
	vec3 normal;
	vec4 color;
} vertexData;

void main()
{
	vertexData.normal = aNormal;
	vertexData.color = aColor;
	gl_Position = uViewProj * vec4(aPos.xyz, 1.0);
}