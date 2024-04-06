#include "GraphicsException.h"

GraphicsException::GraphicsException(int line, const wchar_t* file, std::vector<std::wstring> errorVector)
	:
	StimplyException(line, file, FromVectorToString(errorVector)) {}

GraphicsException::GraphicsException(int line, const wchar_t* file, std::wstring_view reason) 
	:
	StimplyException(line, file, reason) {
}
