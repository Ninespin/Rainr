#version 450 core
in VertexData{
	vec3 normal;
	vec4 color;
} vertexData;

out vec4 FragColor; 

uniform vec3 uSunPos;
const float lightDissipationOffset = 0.3f;
const float ambientIlluminance = 0.2f;




void main()
{
	float illuminance = clamp(dot(normalize(vertexData.normal), normalize(uSunPos)) + lightDissipationOffset, ambientIlluminance, 1.0f);
	FragColor = vertexData.color * illuminance;
}