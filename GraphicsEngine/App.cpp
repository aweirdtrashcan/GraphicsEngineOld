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
	const Graphics::FramePrepData& prepData = m_Graphics.PrepareFrame();
	static float acc = 0.0f;
	acc += prepData.deltaTime;
	m_Triangle->Rotate(0.0f, 0.0f, acc, prepData.frameNumber);
	m_Triangle->RecordDrawCommands(prepData.frameNumber, m_Graphics.GetCommandList());
	m_Graphics.ExecuteCommandLists();
	m_Graphics.Present();
}
