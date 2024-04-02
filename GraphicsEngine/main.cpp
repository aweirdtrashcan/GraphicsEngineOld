#include "App.h"

#include "Win.h"
#include "StimplyException.h"

static inline int run() {
	try {
		return App{}.Run();
	} catch (const StimplyException& exception) {
		MessageBoxW(NULL, exception.Reason(), exception.GetType(), MB_OK | MB_ICONERROR);
	} catch (const std::exception& exception) {
		MessageBoxA(NULL, "Standard Exception", exception.what(), MB_OK | MB_ICONERROR);
	}
	return -1;
}

#if IS_DEBUG
int main() {
	return run();
}
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return run();
}
#endif