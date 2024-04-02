#include "App.h"

App::App() 
	:
	m_Window(800, 600),
	m_Graphics(m_Window.GetWindowRect())
{}

App::~App() {}

int App::Run() {

	while (true) {
		int messageCode = Window::Get().ProcessMessages();

		if (messageCode != 0xF0D45E) {
			return messageCode;
		}
	}

	return 0;
}
