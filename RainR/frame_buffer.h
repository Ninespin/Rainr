#pragma once

#include "i_frame_buffer.h"
#include <map>
#include "texture2d.h"
#include "render_buffer.h"

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

