#pragma once

#include "i_frame_buffer.h"
#include "texture/render_buffer.h"
#include "texture/texture2d.h"
#include <map>

class FrameBuffer: public FrameBufferBase
{
public:
	typedef std::map<GLenum, Texture2D*> TextureAttachmentMap_t;
	typedef std::map<GLenum, RenderBuffer*> RenderBufferAttachmentMap_t;
public:
	FrameBuffer();
	~FrameBuffer();
	void attachTexture2D(GLenum attachmentPoint, Texture2D& texture);
	void attachRenderBuffer(GLenum attachmentPoint, RenderBuffer& renderBuffer);
public:
	TextureAttachmentMap_t mTextureAttachments;
	RenderBufferAttachmentMap_t mRenderBufferAttachments;
};

