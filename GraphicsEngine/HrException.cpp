#include "HrException.h"

#include <sstream>

std::wstring FromVectorToString(const std::vector<std::wstring>& errors) {
	std::wstringstream oss;
	for (size_t i = 0; i < errors.size(); i++) {
		oss << errors[i] << "\n\n";
	}

	return oss.str();
}

HrException::HrException(int line, const wchar_t* file, std::vector<std::wstring> errorVector)
	:
	StimplyException(line, file, FromVectorToString(errorVector))
{}
