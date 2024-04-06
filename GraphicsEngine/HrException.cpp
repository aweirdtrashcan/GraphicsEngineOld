#include "HrException.h"

HrException::HrException(int line, const wchar_t* file, long hresult, const std::vector<std::wstring>& opt_errors)
	:
	StimplyException(line, file, TranslateHr(hresult, opt_errors))
{
}

std::wstring_view HrException::TranslateHr(long hresult, const std::vector<std::wstring>& opt_errors) {
	return std::wstring_view();
}
