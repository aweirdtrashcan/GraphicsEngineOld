#pragma once

#include "Window.h"
#include "Graphics.h"

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
	std::unique_ptr<class Triangle> m_Triangle;
};

