#include "StimplyException.h"

#include <cstring>
#include <sstream>

StimplyException::StimplyException(int line, const wchar_t* file, std::wstring_view reason) {
	std::wstringstream oss;
	
	oss << GetType() << "\n" 
		<< "[LINE]: " << line 
		<< "\n[FILE]: " << file
		<< "\n" << Reason();
	
	m_What = oss.str();
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
