#pragma once

#include "Window.h"
#include "Graphics.h"
#include "Camera.h"
#include <vector>
#include "IDrawable.h"

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
	std::vector<IDrawable> m_Drawables;
	Graphics m_Graphics;
	Camera m_Camera;
};

