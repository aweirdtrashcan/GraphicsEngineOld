#include "App.h"

#include "Win.h"
#include "StimplyException.h"
#include "FreeListAllocator.h"
#include "UUID.h"

#if IS_DEBUG
static inline int run() {
	try {
		return App{}.Run();
	} catch (const StimplyException& exception) {
		std::wcout << L"Exception thrown " << exception.Reason() << L"\n";
		MessageBoxW(NULL, exception.Reason(), exception.GetType(), MB_OK | MB_ICONERROR);
	} catch (const std::exception& exception) {
		std::cout << "Exception thrown " << exception.what() << "\n";
		MessageBoxA(NULL, "Standard Exception", exception.what(), MB_OK | MB_ICONERROR);
	}
	return -1;
}
#else
static inline int run() {
	return App{}.Run();
}
#endif

#if IS_DEBUG
int main() {
	return run();
}
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return run();
}
#endif