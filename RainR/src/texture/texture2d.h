#pragma once
#include "i_texture.h"

class Texture2D: public TextureBase
{
public:
	Texture2D(unsigned int width, unsigned int height, GLint magMode, GLint minMode, GLint internalFormat, GLenum format, GLenum dataType);
	~Texture2D();

protected:
	void allocate();

private:
	unsigned int mWidth, mHeight;
	GLint mInternalFormat, mMagMode, mMinMode;
	GLenum mFormat, mDataType;

};
