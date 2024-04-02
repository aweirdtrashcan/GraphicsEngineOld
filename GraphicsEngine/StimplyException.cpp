#include "StimplyException.h"

#include <cstring>
#include <sstream>

StimplyException::StimplyException(int line, const wchar_t* file, std::wstring_view reason) {
	wchar_t buffer[10000];

	swprintf_s(buffer, L"%s\n[FILE]: %s\n[LINE]: %d\n%s", GetType(), file, line, reason.data());
	m_What = std::wstring(buffer);
}

const wchar_t* StimplyException::GetType() const noexcept {
	return L"Stimply Exception";
}

const char* StimplyException::what() const noexcept {
	return "StimplyException doesn't support what() function because it uses wide string!";
}

const wchar_t* StimplyException::Reason() const noexcept {
	return m_What.c_str();
}
