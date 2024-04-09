#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Camera.h"
#include <vector>

#ifdef PIX_DEBUG
#include "PixGpuCapturer.h"
#endif

class App {
public:
	App();
	~App();

	int Run();

	void DoFrame();

private:
	Window m_Window;
#ifdef PIX_DEBUG
	PixGpuCapturer capturer;
#endif
	Graphics m_Graphics;
	class Triangle** m_Triangle;
	Camera m_Camera;
};

