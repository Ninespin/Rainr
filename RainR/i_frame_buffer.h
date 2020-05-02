#pragma once
#include <GL/glew.h>

class IFrameBuffer
{
public:
	virtual void bind() = 0;
};

class FrameBufferBase: public IFrameBuffer
{
public:
	FrameBufferBase()
		: mHandle(0)
	{
		generate();
	}
	~FrameBufferBase()
	{
		glDeleteFramebuffers(1, &mHandle);
	}

	void bind() override 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	}

protected:
	void generate()
	{
		glGenFramebuffers(1, &mHandle);
	}

public:
	GLuint mHandle;
};