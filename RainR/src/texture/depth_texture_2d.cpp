#include "depth_texture_2d.h"

DepthTexture2D::DepthTexture2D(unsigned int width, unsigned int height, GLint magMode, GLint minMode, 
	GLint internalFormat = GL_DEPTH24_STENCIL8, 
	GLenum format = GL_DEPTH_STENCIL, GLenum dataType = GL_UNSIGNED_INT_24_8)
	: Texture2D(width, height, magMode, minMode, internalFormat, format, dataType)
{
}