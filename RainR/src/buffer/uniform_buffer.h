#pragma once
#include <GL/glew.h>


template <typename T>
struct __declspec(align(256)) Aligned_t
{
	 T value;
};

template <typename T>
class UniformBuffer
{
public:
	UniformBuffer(unsigned int bindingPoint, unsigned int usage, unsigned int dataCount = 0, T* data = nullptr)
		: _aligned_sizeof_T(uboAligned(sizeof(Aligned_t<T>)))
		, mHandle(0)
		, mBindingPoint(bindingPoint)
		, mUsage(usage)
		, mData()
	{
		if (dataCount * sizeof(Aligned_t<T>) > 0)
		{
			for(unsigned int i = 0; i < dataCount; i++)
			{
				mData.push_back({ data[i] });
			}
			recompile();
		}
	}

	~UniformBuffer()
	{
		glDeleteBuffers(1, &mHandle);
	}

	void bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, mBindingPoint, mHandle);
	}

	void bindRange(unsigned int offset_index)
	{
		const unsigned int offset = offset_index * _aligned_sizeof_T;
		glBindBufferRange(GL_UNIFORM_BUFFER, mBindingPoint, mHandle, offset, sizeof(Aligned_t<T>));
	}

	void unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void recompile()
	{
		glDeleteBuffers(1, &mHandle);
		glGenBuffers(1, &mHandle);
		bind();
		glBufferData(GL_UNIFORM_BUFFER, mData.size() * _aligned_sizeof_T, nullptr, mUsage);
		update(0, mData.size(), mData.data());
	}

	void update(unsigned int offset_index, unsigned int count, Aligned_t<T>* data)
	{
		bind();
		const unsigned int offset = offset_index * _aligned_sizeof_T;
		const unsigned int sizeInBytes = count * _aligned_sizeof_T;
		char* bufferData = reinterpret_cast<char*>(glMapBufferRange(GL_UNIFORM_BUFFER, offset, sizeInBytes, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		for(unsigned int i = offset_index, addr = offset; i < count; i++, addr += _aligned_sizeof_T)
		{
			memcpy(&bufferData[addr], &data[i], sizeof(T));
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);

	}

	static inline size_t uboAligned(size_t size)
	{
		int alignment;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
		return ((size + alignment - 1) / alignment) * alignment;
	}

public:
	const unsigned int _aligned_sizeof_T;
	GLuint mHandle;
	GLuint mBindingPoint;
	GLuint mUsage;
	std::vector<Aligned_t<T>> mData;
};


