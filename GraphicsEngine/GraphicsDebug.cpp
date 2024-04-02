#include "GraphicsDebug.h"

#include "StimplyException.h"
#include "Macros.h"
#include "HrException.h"

#pragma comment(lib, "dxguid.lib")

GraphicsDebug::GraphicsDebug() {
	HMODULE debug = GetModuleHandleA("dxgidebug.dll");
	if (!debug) {
		STIMPLY_EXCEPTION(L"Failed to query dxgidebug.dll");
	}

	typedef HRESULT(DXGIGetDebugInterface)(REFIID riid, void** ppDebug);
	DXGIGetDebugInterface* func = (DXGIGetDebugInterface*)GetProcAddress(debug, "DXGIGetDebugInterface");
	
	func(IID_PPV_ARGS(&m_InfoQueue));
	
	FreeModule(debug);
}

void GraphicsDebug::Clear() {
	GraphicsDebug& dbg = GraphicsDebug::Get();
}

static std::wstring AnsiToWide(const char* ansi) {
	size_t strLen = strlen(ansi);
	std::wstring wstr;
	wstr.resize(strLen);
	for (size_t i = 0; i < strLen; i++) {
		wstr[i] = ansi[i];
	}
	return wstr;
}

std::vector<std::wstring> GraphicsDebug::GetErrors() {
	GraphicsDebug& dbg = GraphicsDebug::Get();

	UINT storedMessages = dbg.m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_D3D12);
	if (storedMessages <= 0) {
		return {};
	}
	for (UINT i = 0; i < storedMessages; i++) {
		SIZE_T messageSize = 0;
		(dbg.m_InfoQueue->GetMessageW(DXGI_DEBUG_D3D12, i, nullptr, &messageSize));
		DXGI_INFO_QUEUE_MESSAGE* message = (DXGI_INFO_QUEUE_MESSAGE*)malloc(messageSize * 2);
		memset(message, 0, messageSize * 2);
		(dbg.m_InfoQueue->GetMessageW(DXGI_DEBUG_D3D12, i, message, &messageSize));
		dbg.m_ErrorList.push_back(AnsiToWide(message->pDescription));
		free(message);
	}
	return dbg.m_ErrorList;
}
