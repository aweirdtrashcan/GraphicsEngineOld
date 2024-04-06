#include "Window.h"

#include "StimplyException.h"
#include "Macros.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_win32.h"

bool Window::s_Initialized = false;
Window* Window::s_WindowInstance = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window(UINT width, UINT height)
	:
	m_hInstance(GetModuleHandleW(0)),
	m_WindowWidth(width),
	m_WindowHeight(height) {

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

	UINT offset = 100;
	RECT windowRect;
	windowRect.left = offset;
	windowRect.right = m_WindowWidth + offset;
	windowRect.top = offset;
	windowRect.bottom = m_WindowHeight + offset;

	AdjustWindowRectEx(
		&windowRect,
		WS_OVERLAPPEDWINDOW,
		FALSE,
		createWindowFlags
	);

	HWND hwnd = CreateWindowExW(
		createWindowFlags,
		L"EngineClassName",
		L"StimplyEngine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, NULL, NULL,
		m_hInstance, NULL
	);

	if (!hwnd) {
		STIMPLY_EXCEPTION(GetWindowsError());
	}

	m_hWnd = hwnd;

	ShowWindow(m_hWnd, SW_SHOW);
	
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_hWnd);
	
	ImGui::StyleColorsDark();

	s_WindowInstance = this;
	s_Initialized = true;
}

Window::~Window() {
	s_Initialized = false;
	s_WindowInstance = nullptr;
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	UnregisterClassW(L"EngineClassName", m_hInstance);
}

int Window::ProcessMessages() const noexcept(!IS_DEBUG) {
	static MSG msg;
	memset(&msg, 0, sizeof(msg));

	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		if (msg.message == WM_QUIT) {
			return 0;
		}
	}
	return 0xF0D45E;
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
	const ImGuiIO& imio = ImGui::GetIO();

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return true;
	}

	if (imio.WantCaptureKeyboard || imio.WantCaptureMouse) {
		return true;
	}

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
