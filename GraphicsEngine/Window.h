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

	int ProcessMessages() const noexcept(!IS_DEBUG);
	HWND GetHandleToWindow() const noexcept(!IS_DEBUG) { return m_hWnd; }
	static const Window* Get();
	UINT GetWindowWidth() const { return m_WindowWidth; }
	UINT GetWindowHeight() const { return m_WindowHeight; }

private:
	static LRESULT CALLBACK sWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	const std::wstring GetWindowsError() const;

private:
	HINSTANCE m_hInstance = 0;
	HWND m_hWnd = 0;
	static Window* s_WindowInstance;
	UINT m_WindowWidth;
	UINT m_WindowHeight;
};