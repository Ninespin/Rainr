#include "render_pass.h"
#include "camera/camera.h"
#include "buffer/frame_buffer.h"

void RenderPass::render()
{
	if(m_frameBuffer)
	{
		m_frameBuffer->bind();
	}



	if (m_frameBuffer)
	{
		m_frameBuffer->unbind();
	}
}
