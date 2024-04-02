#include "Window.h"

#include "StimplyException.h"
#include "Macros.h"

bool Window::s_Initialized = false;
Window* Window::s_WindowInstance = nullptr;

Window::Window(UINT width, UINT height)
	:
	m_hInstance(GetModuleHandleW(0)) {

	if (s_Initialized) {
		STIMPLY_EXCEPTION(L"Trying to initialize Window more than once.");
	}

	WNDCLASSEXW winClass = {};
	winClass.cbClsExtra = 0;
	winClass.cbSize = sizeof(winClass);
	winClass.hInstance = m_hInstance;
	winClass.lpszClassName = L"EngineClassName";
	winClass.cbWndExtra = 0;
	winClass.hbrBackground = NULL;
	winClass.hCursor = NULL;
	winClass.hIcon = NULL;
	winClass.hIconSm = NULL;
	winClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = sWinProc;

	ATOM success = RegisterClassExW(&winClass);
	
	if (!success) {
		STIMPLY_EXCEPTION(GetWindowsError());
	}

	DWORD createWindowFlags = WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW;

	m_WindowRect.left = 0;
	m_WindowRect.right = width;
	m_WindowRect.top = 0;
	m_WindowRect.bottom = height;

	HWND hwnd = CreateWindowExW(
		createWindowFlags,
		L"EngineClassName",
		L"StimplyEngine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height, NULL, NULL,
		m_hInstance, NULL
	);

	AdjustWindowRectEx(
		&m_WindowRectOffset,
		WS_OVERLAPPEDWINDOW,
		FALSE,
		createWindowFlags
	);

	m_WindowRect.bottom -= m_WindowRectOffset.bottom;
	m_WindowRect.left -= m_WindowRectOffset.left;
	m_WindowRect.top -= m_WindowRectOffset.top;
	m_WindowRect.right -= m_WindowRectOffset.right;

	if (!hwnd) {
		STIMPLY_EXCEPTION(GetWindowsError());
	}

	m_hWnd = hwnd;

	ShowWindow(m_hWnd, SW_SHOW);
	
	s_WindowInstance = this;
	s_Initialized = true;
}

Window::~Window() {
	DestroyWindow(m_hWnd);
	UnregisterClassW(L"EngineClassName", m_hInstance);
}

int Window::ProcessMessages() const noexcept {
	static MSG msg;
	memset(&msg, 0, sizeof(msg));

	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
		TranslateMessage(&msg);

		if (msg.message == WM_QUIT) {
			return 0;
		}

		DispatchMessageW(&msg);
	}
	return 0xF0D45E;
}

RECT Window::GetWindowRect() const noexcept {
	RECT rect;
	rect.left = m_WindowRect.left + m_WindowRectOffset.left;
	rect.right = m_WindowRect.right + m_WindowRectOffset.right;
	rect.bottom = m_WindowRect.bottom + m_WindowRectOffset.bottom;
	rect.top = m_WindowRect.top + m_WindowRectOffset.top;
	return rect;
}

Window& Window::Get() noexcept(!IS_DEBUG) {
	if (!s_WindowInstance)
		STIMPLY_EXCEPTION(L"Trying to get a window instance that is not yet initialized!");

	return *s_WindowInstance;
}

LRESULT Window::sWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (s_Initialized) {
		return Window::Get().WinProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Window::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CLOSE: 
		{
			DestroyWindow(hWnd);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

const std::wstring Window::GetWindowsError() const {
	DWORD error = GetLastError();
	
	LPWSTR errorBuffer = nullptr;

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		error,
		NULL,
		errorBuffer,
		10,
		NULL
	);

	return errorBuffer;
}
