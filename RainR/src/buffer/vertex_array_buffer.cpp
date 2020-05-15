#include "vertex_array_buffer.h"


VertexArrayBuffer::VertexArrayBuffer()
	: mHandle(0)
{
	glGenVertexArrays(1, &mHandle);
	bind();
}

VertexArrayBuffer::~VertexArrayBuffer()
{
	unbind();
	glDeleteVertexArrays(1, &mHandle);
}

void VertexArrayBuffer::bind()
{
	glBindVertexArray(mHandle);
}

void VertexArrayBuffer::unbind()
{
	glBindVertexArray(0);
}