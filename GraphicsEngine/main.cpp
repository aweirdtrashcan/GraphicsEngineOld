#include "App.h"

#include "Win.h"
#include "StimplyException.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	try {
		return App{}.Run();
	}
	catch (const StimplyException& exception) {
		MessageBox(NULL, exception.GetType(), exception.Reason(), MB_OK);
	}
	catch (const std::exception& exception) {
		MessageBoxA(NULL, "Standard Exception", exception.what(), MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}