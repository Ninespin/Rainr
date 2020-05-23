#version 450 core
in VertexData{
	vec3 normal;
	vec4 color;
} vertexData;

out vec4 FragColor; 

uniform vec3 uSunPos;

layout(std140,binding=1) uniform MaterialBuffer {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	vec3 emissive;
	float opacity;
	float shininess;
	float shininessStrength;
} uMaterial;

const float lightDissipationOffset = 0.3f;

void main()
{
	float illuminance = dot(normalize(vertexData.normal), normalize(uSunPos)) + lightDissipationOffset;
	FragColor = vertexData.color * vec4(uMaterial.diffuse, uMaterial.opacity) * illuminance + vec4(uMaterial.ambient, 0)+ vec4(uMaterial.emissive, 0);
}