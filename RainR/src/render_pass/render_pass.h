#pragma once
#include <vector>

class Camera;
class FrameBuffer;

class RenderPass
{
public:
	virtual void setup() = 0;
	virtual void render();


public:
	std::vector<Camera*> m_cameras;
	FrameBuffer* m_frameBuffer;
};