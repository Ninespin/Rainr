#pragma once
#include <GL/glew.h>

template <typename  T>
class VertexBufferObject
{
public:
	VertexBufferObject(unsigned int bufferType, unsigned int drawType, unsigned int glDataType, size_t dataSegmentation, size_t dataCount, T* data,
		bool normalized, unsigned int attribLocation);
	~VertexBufferObject();
	void bind() const;
	void unbind() const;
	void enable() const;
	void disable() const;

protected:
	GLenum mBufferType;
	size_t mDataCount;
	T* mDataPointer;
	GLuint mAttribBindPoint;
	GLenum mGLDataType;
	GLboolean mNormalized;
	GLenum mDrawType;
	GLuint mHandle;
	GLuint mDataSegmentation; // number of mGLDataTypes per T

};

template <typename T>
VertexBufferObject<T>::VertexBufferObject(unsigned int bufferType, unsigned int drawType, unsigned int glDataType, size_t dataSegmentation, size_t dataCount, T* data,
	bool normalized, unsigned int attribLocation)
	: mBufferType(bufferType)
	, mDataCount(dataCount)
	, mDataPointer(data)
	, mAttribBindPoint(attribLocation)
	, mGLDataType(glDataType)
	, mNormalized(normalized)
	, mDrawType(drawType)
	, mHandle(0)
	, mDataSegmentation(dataSegmentation)
{
	glGenBuffers(1, &mHandle);
	bind();
	glBufferData(mBufferType, mDataCount * sizeof(T),
		mDataPointer, mDrawType);
	glVertexAttribPointer(mAttribBindPoint, mDataSegmentation, mGLDataType, mNormalized, sizeof(T), nullptr);
	enable();

}

template <typename T>
VertexBufferObject<T>::~VertexBufferObject()
{
	glDeleteBuffers(1, &mHandle);
}

template <typename T>
void VertexBufferObject<T>::bind() const
{
	glBindBuffer(mBufferType, mHandle);
}

template <typename T>
void VertexBufferObject<T>::unbind() const
{
	glBindBuffer(mBufferType, 0);
}

template <typename T>
void VertexBufferObject<T>::enable() const
{
	glEnableVertexAttribArray(mAttribBindPoint);
}

template <typename T>
void VertexBufferObject<T>::disable() const
{
	glDisableVertexAttribArray(mAttribBindPoint);
}


