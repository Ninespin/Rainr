#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aColor;
uniform mat4 uViewProj;
out vec3 normal;
out vec4 color;

void main()
{
	normal = aNormal;
	color = aColor;
	gl_Position = uViewProj * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}