#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
uniform mat4 uViewProj;
uniform mat4 uModelMat;

layout(std140, binding=0) uniform MatrixBuffer {
	mat4 modelMat;
	mat4 padd[3];
} uMatrices;

out VertexData{
	vec3 normal;
	vec4 color;
} vertexData;

void main()
{
	vertexData.normal = aNormal;
	vertexData.color = vec4(clamp(aColor.xyz, 0.2, 1.0), aColor.a);
	gl_Position = uViewProj * (uMatrices.modelMat) * vec4(aPos.xyz, 1.0);
}