#version 450 core
in VertexData{
	vec3 normal;
	vec4 color;
} vertexData;

out vec4 FragColor; 

uniform vec3 uSunPos;
const float ambientIlluminance = 0.15;



void main()
{
	float illuminance = clamp(dot(vertexData.normal, uSunPos), ambientIlluminance, 1.0f);
	FragColor = vertexData.color * illuminance;
}