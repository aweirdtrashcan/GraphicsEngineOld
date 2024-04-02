#pragma once

#include "Window.h"
#include "Graphics.h"

class App {
public:
	App();
	~App();

	int Run();

private:
	Window m_Window;
	Graphics m_Graphics;
};

