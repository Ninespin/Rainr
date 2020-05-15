#pragma once
#include <GL/glew.h>

class VertexArrayBuffer
{
public:
	VertexArrayBuffer();
	~VertexArrayBuffer();
	void bind();
	void unbind();

public:
	GLuint mHandle;
};