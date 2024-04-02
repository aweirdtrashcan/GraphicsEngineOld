#pragma once

#include "D3DIncls.inl"
#include <vector>
#include <string>

class GraphicsDebug {
public:
#if IS_DEBUG
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
#endif
};

