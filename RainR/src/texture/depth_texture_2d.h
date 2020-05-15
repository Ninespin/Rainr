#pragma once
#include "texture2d.h"
class DepthTexture2D : public Texture2D
{
public:
	DepthTexture2D(unsigned int width, unsigned int height, GLint magMode, 
		GLint minMode, GLint internalFormat, 
		GLenum format, GLenum dataType);
};

