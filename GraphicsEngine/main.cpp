#include "App.h"

#include "Win.h"
#include "StimplyException.h"

#if IS_DEBUG
int main(void) {
	try {
		return App{}.Run();
	} catch (const StimplyException& exception) {
		MessageBoxW(NULL, exception.GetType(), exception.Reason(), MB_OK | MB_ICONEXCLAMATION);
		wprintf(L"%ws\n", exception.Reason());
	} catch (const std::exception& exception) {
		MessageBoxA(NULL, "Standard Exception", exception.what(), MB_OK | MB_ICONEXCLAMATION);
		printf("%s\n", exception.what());
	}

	return -1;
}
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	try {
		return App{}.Run();
	} catch (const StimplyException& exception) {
		MessageBoxW(NULL, exception.GetType(), exception.Reason(), MB_OK | MB_ICONEXCLAMATION);
	} catch (const std::exception& exception) {
		MessageBoxA(NULL, "Standard Exception", exception.what(), MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
#endif