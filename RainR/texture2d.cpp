#include "texture2d.h"



Texture2D::Texture2D(unsigned int width, unsigned int height, GLint magMode = GL_NEAREST, GLint minMode = GL_NEAREST, GLint internalFormat = GL_RGBA, GLenum format = GL_RGBA, GLenum dataType = GL_FLOAT)
	: TextureBase(GL_TEXTURE_2D)
	, mWidth(width)
	, mHeight(height)
	, mMagMode(magMode)
	, mMinMode(minMode)
	, mInternalFormat(internalFormat)
	, mFormat(format)
	, mDataType(dataType)
{
	TextureBase::bind();
	allocate();
	setParameter(GL_TEXTURE_MAG_FILTER, mMagMode);
	setParameter(GL_TEXTURE_MIN_FILTER, mMinMode);
	TextureBase::unbind();
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &mHandle);
}

void Texture2D::allocate()
{
	glTexImage2D(mTextureType, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDataType, 0);
}


