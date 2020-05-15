#include "frame_buffer.h"


FrameBuffer::FrameBuffer() = default;

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::attachTexture2D(GLenum attachmentPoint, Texture2D& texture)
{
	mTextureAttachments[attachmentPoint] = &texture;
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, texture.mHandle, 0);
}

void FrameBuffer::attachRenderBuffer(GLenum attachmentPoint, RenderBuffer& renderBuffer)
{
	mRenderBufferAttachments[attachmentPoint] = &renderBuffer;
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentPoint, GL_RENDERBUFFER, renderBuffer.mHandle);
}
