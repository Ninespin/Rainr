#pragma once

#include <GL/glew.h>

class ITexture
{
public:
	virtual void bind() = 0;
	virtual void unbind() = 0;
protected:
};

class TextureBase: public ITexture
{
public:
	TextureBase(GLenum texType)
		: mTextureType(texType)
		, mHandle(0)
	{
		generate();
	}
	~TextureBase()
	{
		glDeleteTextures(1, &mHandle);
	}

	void bind() override 
	{
		glBindTexture(GL_TEXTURE_2D, mHandle);
	}

	void unbind() override
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void setParameter(GLenum parameter, GLint value) const
	{
		glTexParameteri(mTextureType, parameter, value);
	}

	void setParameter(GLenum parameter, GLfloat value) const
	{
		glTexParameterf(mTextureType, parameter, value);
	}

protected:
	void generate()
	{
		glGenTextures(1, &mHandle);
	}


public:
	GLuint mHandle;
	GLenum mTextureType;

};

