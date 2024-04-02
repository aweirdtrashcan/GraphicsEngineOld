#pragma once

#include <exception>
#include <string>


class StimplyException {
public:
	StimplyException(int line, const wchar_t* file, std::wstring_view reason);
	virtual ~StimplyException() = default;
	virtual const wchar_t* GetType() const noexcept;
	virtual const char* what() const noexcept;
	virtual const wchar_t* Reason() const noexcept;
	
protected:
	std::wstring m_What;
};

