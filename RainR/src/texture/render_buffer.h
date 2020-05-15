#pragma once
#include "texture/i_texture.h"

/// Render buffers do not need to worry about internal format (uses internal gpu format)
/// which makes them faster for off-screen renders.
class RenderBuffer : public ITexture
{
public:
	RenderBuffer(unsigned int width, unsigned int height, GLenum format);
	~RenderBuffer();
	void unbind() override;
	void bind() override;
	
protected:
	void generate();
	void allocate();
public:
	GLuint mHandle;
	unsigned int mWidth, mHeight;
	GLenum mFormat;
};

