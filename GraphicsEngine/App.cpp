#include "App.h"

#include "Triangle.h"
#include "Suzanne.h"

App::App() 
	:
	m_Window(1280, 720),
	m_Graphics(m_Window.GetWindowWidth(), m_Window.GetWindowHeight()),
	m_Camera((float)m_Window.GetWindowWidth(), (float)m_Window.GetWindowHeight())
{
	m_Drawables.push_back(Suzanne("Models/suzanne.obj"));
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

	m_Camera.Update(prepData.frameNumber);

	//float xx[4] = { 3.0f, 1.0f, -1.0f, -3.0f };

	//for (size_t i = 0; i < 4; i++) {
	//	auto& t = m_Triangle[i];
	//	t->Rotate(0.0f, 0.0f, acc);
	//	t->Move(xx[i], 0.0f, 0.0f);

	//	t->Update(m_Camera, prepData.frameNumber);
	//	t->RecordDrawCommands(prepData.frameNumber, m_Graphics.GetCommandList(), m_Camera);
	//}


	for (size_t i = 0; i < m_Drawables.size(); i++) {
		m_Drawables[i].Update(m_Camera, prepData.frameNumber);
		m_Drawables[i].RecordDrawCommands(prepData.frameNumber, m_Graphics.GetCommandList(), m_Camera);
	}

	m_Graphics.ExecuteCommandLists();
	m_Graphics.Present();
}
