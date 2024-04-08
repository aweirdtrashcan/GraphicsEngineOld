#include "App.h"

#include "Triangle.h"

App::App() 
	:
	m_Window(1280, 720),
	m_Graphics(m_Window.GetWindowWidth(), m_Window.GetWindowHeight())
{
	m_Triangle = std::make_unique<Triangle>();
}

App::~App() {
}

int App::Run() {
	int messageCode = 0xF0D45E;
	while (messageCode == 0xF0D45E) {
		messageCode = Window::Get()->ProcessMessages();

		DoFrame();
	}

	m_Graphics.WaitDeviceIdle();

	return 0;
}

void App::DoFrame() {
	UINT frameNum = m_Graphics.PrepareFrame();
	m_Triangle->RecordDrawCommands(frameNum, m_Graphics.GetCommandList());
	m_Graphics.ExecuteCommandLists();
	m_Graphics.Present();
}
