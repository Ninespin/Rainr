#pragma once
#include "vertex_buffer_object.h"

template <typename T>
class ShaderStorageBuffer
{
public:
	ShaderStorageBuffer(size_t dataCount, T* data,
		GLenum usage, unsigned int binding)
		: mHandle(0)
		, mDataCount(dataCount)
		, mData(data)
		, mUsage(usage)
		, mBaseBinding(binding)
	{
		glGenBuffers(1, &mHandle);
		bind();
		glBufferData(GL_SHADER_STORAGE_BUFFER, mDataCount * sizeof(T), mData, mUsage);

	}
	~ShaderStorageBuffer()
	{
		unbind();
		glDeleteBuffers(1, &mHandle);
	}

	void bind()
	{
		bind(mBaseBinding);
	}

	void bind(unsigned int bindingbase)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingbase, mHandle);
	}

	void unbind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	T* const map(unsigned int offset = 0, unsigned int bufferMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)
	{
		return (T*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, offset, mDataCount * sizeof(T), bufferMask);
	}

	void unmap()
	{
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	void bindAsVBO(unsigned int attibPointer, unsigned int nbComponents, GLenum componentType = GL_FLOAT, bool normalize = false, unsigned int attribDivisor = 0)
	{
		bind();
		glBindBuffer(GL_ARRAY_BUFFER, mHandle);
		glVertexAttribPointer(attibPointer, nbComponents, componentType, normalize, sizeof(T), nullptr);
		glEnableVertexAttribArray(attibPointer);
		glVertexAttribDivisor(attibPointer, attribDivisor);
	}

	void unbindAsVBO()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

public:
	GLuint mHandle;
	size_t mDataCount;
	T* mData;
	GLenum mUsage;
	unsigned int mBaseBinding;
	
};
