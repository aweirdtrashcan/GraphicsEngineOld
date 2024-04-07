#include "GraphicsDebug.h"

#include "StimplyException.h"
#include "Macros.h"
#include "HrException.h"

#pragma comment(lib, "dxguid.lib")

#if IS_DEBUG
#include <iostream>
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

	UINT64 storedMessages = dbg.m_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_DX);
	if (storedMessages <= 0) {
		return {};
	}
	for (UINT64 i = 0; i < storedMessages; i++) {
		SIZE_T messageSize = 0;
		dbg.m_InfoQueue->GetMessageW(DXGI_DEBUG_DX, i, nullptr, &messageSize);
		DXGI_INFO_QUEUE_MESSAGE* message = (DXGI_INFO_QUEUE_MESSAGE*)new char[messageSize];
		dbg.m_InfoQueue->GetMessageW(DXGI_DEBUG_DX, i, message, &messageSize);
		
		// less than 2 = either error or corruption.
		if (message->Severity < 2) {
			dbg.m_ErrorList.push_back(AnsiToWide(message->pDescription));
		} else {
			std::cout << message->pDescription << "\n";
		}

		delete[] message;
	}
	return dbg.m_ErrorList;
}
#endif