#pragma once

#include "D3DIncls.inl"
#include <vector>
#include <string>

class GraphicsDebug {
public:
	GraphicsDebug(const GraphicsDebug&) = delete;
	GraphicsDebug(GraphicsDebug&&) = delete;

	static void Clear();
	static std::vector<std::wstring> GetErrors();

	static GraphicsDebug& Get() {
		static GraphicsDebug instance;
		return instance;
	}

private:
	GraphicsDebug();
	~GraphicsDebug() = default;

	std::vector<std::wstring> m_ErrorList;
	ComPtr<IDXGIInfoQueue> m_InfoQueue;
};

