#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 4) in vec4 aOffs;
out vec4 color;

void main()
{
	vec3 pos = aPos + aOffs.xyz;
	color = aColor;
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}