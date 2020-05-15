#include "render_buffer.h"



RenderBuffer::RenderBuffer(unsigned int width, unsigned int height, GLenum format)
	: mHandle(0)
	, mWidth(width)
	, mHeight(height)
	, mFormat(format)
{
	generate();
	RenderBuffer::bind();
	allocate();
	RenderBuffer::unbind();

}


RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &mHandle);
}

void RenderBuffer::bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, mHandle);
}

void RenderBuffer::unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderBuffer::generate()
{
	glGenRenderbuffers(1, &mHandle);
}

void RenderBuffer::allocate()
{
	glRenderbufferStorage(GL_RENDERBUFFER, mFormat, mWidth, mHeight);
}
