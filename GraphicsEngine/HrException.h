#pragma once

#include "StimplyException.h"
#include <vector>

class HrException : public StimplyException {
public:
	HrException(int line, const wchar_t* file, std::vector<std::wstring> errorVector);
	virtual const wchar_t* GetType() const noexcept { return L"HR Exception"; }
};

