#pragma once

#include "StimplyException.h"

#include <vector>
#include <string>
#include <sstream>

class GraphicsException : public StimplyException {
public:
	GraphicsException(int line, const wchar_t* file, std::vector<std::wstring> errorVector);
	GraphicsException(int line, const wchar_t* file, std::wstring_view reason);
	virtual const wchar_t* GetType() const noexcept { return L"Graphics Exception"; }

	static std::wstring FromVectorToString(const std::vector<std::wstring>& errors) {
		std::wstringstream oss;
		oss << "\n\n";
		for (size_t i = 0; i < errors.size(); i++) {
			oss << errors[i] << "\n\n";
		}

		return oss.str();
	}

	static std::wstring FromVectorToString(const std::vector<std::wstring>* errors) {
		if (errors != nullptr) {
			return FromVectorToString(*errors);
		}
	}
};

