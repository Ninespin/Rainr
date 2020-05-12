#version 450 core
in vec3 normal;
in vec4 color;
out vec4 FragColor; 

uniform vec3 uSunPos;
const float ambientIlluminance = 0.4;

void main()
{
	float illuminance = clamp(dot(normal, uSunPos), ambientIlluminance, 1);
	FragColor = color * illuminance; 
}