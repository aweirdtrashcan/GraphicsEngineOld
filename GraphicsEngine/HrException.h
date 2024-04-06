#pragma once

#include "StimplyException.h"
#include <vector>

class HrException : public StimplyException {
public:
	HrException(int line, const wchar_t* file, long hresult, const std::vector<std::wstring>& opt_errors);
	virtual const wchar_t* GetType() const noexcept { return L"HR Exception"; }
	
private:
	std::wstring TranslateHr(long hresult, const std::vector<std::wstring>& opt_errors);
};

