#include "HrException.h"

#include <comdef.h>
#include <sstream>

#pragma comment(lib, "comsuppd.lib")

HrException::HrException(int line, const wchar_t* file, long hresult, const std::vector<std::wstring>& opt_errors)
	:
	StimplyException(line, file, TranslateHr(hresult, opt_errors)) {

	

}

std::wstring HrException::TranslateHr(long hresult, const std::vector<std::wstring>& opt_errors) {
	const TCHAR* err = _com_error(hresult).ErrorMessage();

	std::wstringstream ss;

	ss << err;

	for (const std::wstring& s : opt_errors) {
		ss << s;
	}

	return ss.str();
}
