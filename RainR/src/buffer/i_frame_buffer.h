#pragma once
#include <GL/glew.h>

class IFrameBuffer
{
public:
	virtual void bind() = 0;
	virtual void unbind() = 0;
};

class FrameBufferBase: public IFrameBuffer
{
public:
	FrameBufferBase()
		: mHandle(0)
		, mIsValid(false)
	{
		generate();
		FrameBufferBase::bind();
		
		FrameBufferBase::unbind();

	}
	~FrameBufferBase()
	{
		if(mIsValid)
		{
			glDeleteFramebuffers(1, &mHandle);
		}
	}

	void bind() override 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	}

	void unbind() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool isValid() const
	{
		return mIsValid;
	}

	bool checkCompleteness() const
	{
		return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

protected:
	GLuint generate() 
	{
		if(!mIsValid)
		{
			glGenFramebuffers(1, &mHandle);
			mIsValid = true;
		}
		return mHandle;
	}

public:
	GLuint mHandle;
protected:
	bool mIsValid;
};