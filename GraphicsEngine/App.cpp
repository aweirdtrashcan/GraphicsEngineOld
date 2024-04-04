#include "App.h"

#include "Triangle.h"

App::App() 
	:
	m_Window(800, 600),
	m_Graphics(m_Window.GetWindowRect())
{
	m_Triangle = std::make_unique<Triangle>();
}

App::~App() {
}

int App::Run() {
	int messageCode = 0xF0D45E;
	while (messageCode == 0xF0D45E) {
		messageCode = Window::Get().ProcessMessages();

		DoFrame();
	}

	m_Graphics.WaitDeviceIdle();

	return 0;
}

void App::DoFrame() {
	m_Graphics.PrepareFrame();
	ID3D12CommandList* cmd = m_Triangle->RecordDrawCommands();
	m_Graphics.ExecuteCommandLists(&cmd, 1);
	m_Graphics.Present();
}
