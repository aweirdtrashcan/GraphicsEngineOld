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


	DXGI_INFO_QUEUE_MESSAGE_CATEGORY categories[] = {
		DXGI_INFO_QUEUE_MESSAGE_CATEGORY_INITIALIZATION,
		DXGI_INFO_QUEUE_MESSAGE_CATEGORY_EXECUTION,
		DXGI_INFO_QUEUE_MESSAGE_CATEGORY_SHADER,
		DXGI_INFO_QUEUE_MESSAGE_CATEGORY_CLEANUP
	};
	DXGI_INFO_QUEUE_MESSAGE_SEVERITY severities[] = {
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR,
		//DXGI_INFO_QUEUE_MESSAGE_SEVERITY_INFO, descomenta pra ver o spam que o ImGui produz =D
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY_MESSAGE,
		DXGI_INFO_QUEUE_MESSAGE_SEVERITY_WARNING
	};

	DXGI_INFO_QUEUE_FILTER infoFilter{};
	infoFilter.AllowList.pCategoryList = categories;
	infoFilter.AllowList.NumCategories = _countof(categories);
	infoFilter.AllowList.pSeverityList = severities;
	infoFilter.AllowList.NumSeverities = _countof(severities);

	m_InfoQueue->PushStorageFilter(DXGI_DEBUG_DX, &infoFilter);
	
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