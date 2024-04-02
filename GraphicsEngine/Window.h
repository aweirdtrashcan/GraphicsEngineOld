#pragma once

#include "Win.h"
#include <string>

class Window
{
public:
	Window(UINT width, UINT height);
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	~Window();

	int ProcessMessages() const noexcept;
	RECT GetWindowRect() const noexcept;
	HWND GetHandleToWindow() const noexcept { return m_hWnd; }
	static Window& Get() noexcept(!IS_DEBUG);

private:
	static LRESULT CALLBACK sWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	const std::wstring GetWindowsError() const;

private:
	HINSTANCE m_hInstance = 0;
	HWND m_hWnd = 0;
	static bool s_Initialized;
	static Window* s_WindowInstance;
	RECT m_WindowRect{};
	RECT m_WindowRectOffset{};
};