#version 450 core

in vec2 uv;

layout(location=0) uniform sampler2D screenTexture;

out vec4 FragColor; 

void main()
{
	FragColor = texture(screenTexture, uv); 

}
