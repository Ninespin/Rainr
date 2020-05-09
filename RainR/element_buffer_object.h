#pragma once
#include <GL/glew.h>

template <typename  T>
class ElementBufferObject
{
public:
	ElementBufferObject(unsigned int drawType, size_t dataCount, T* data);
	~ElementBufferObject();
	void bind() const;
	void unbind() const;
	void enable() const;
	void disable() const;

protected:
	GLenum mBufferType;
	size_t mDataCount;
	T* mDataPointer;
	GLenum mDrawType;
	GLuint mHandle;

};

template <typename T>
ElementBufferObject<T>::ElementBufferObject(unsigned int drawType, size_t dataCount, T* data)
	: mBufferType(GL_ELEMENT_ARRAY_BUFFER)
	, mDataCount(dataCount)
	, mDataPointer(data)
	, mDrawType(drawType)
	, mHandle(0)
{
	glGenBuffers(1, &mHandle);
	bind();
	glBufferData(mBufferType, mDataCount * sizeof(T),
		mDataPointer, mDrawType);

}

template <typename T>
ElementBufferObject<T>::~ElementBufferObject()
{
	glDeleteBuffers(1, &mHandle);
}

template <typename T>
void ElementBufferObject<T>::bind() const
{
	glBindBuffer(mBufferType, mHandle);
}

template <typename T>
void ElementBufferObject<T>::unbind() const
{
	glBindBuffer(mBufferType, 0);
}



#pragma once
