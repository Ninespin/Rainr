#version 450 core
in vec3 normal;
in vec4 color;
out vec4 FragColor; 

const vec3 sunPos = vec3(-100, 100, 10);
const float ambientIlluminance = 0.5;

void main()
{
	float illuminance = clamp(dot(normal, sunPos), ambientIlluminance, 1);
	FragColor = color * illuminance; 
}