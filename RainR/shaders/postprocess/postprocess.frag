#version 450 core
#extension GL_ARB_bindless_texture: enable

in vec2 uv;


uniform sampler2D uScreenTexture;

out vec4 FragColor; 

void main()
{
	FragColor = texture(uScreenTexture, uv); 

}
