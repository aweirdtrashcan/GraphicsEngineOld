#pragma once

#include "D3DIncls.inl"
#include "StimplyException.h"
#include <sstream>

class Shader {
public:
	enum class Type {
		VertexShader,
		PixelShader
	};
public:
	class ShaderException : public StimplyException {
	public:
		ShaderException(int line, const wchar_t* file, std::wstring_view shaderPath) :
			StimplyException(line, file, BuildShaderErrorMessage(shaderPath)) {}

		static std::wstring BuildShaderErrorMessage(std::wstring_view shaderPath) {
			std::wstringstream ss;

			ss << L"Failed to load shader:" << shaderPath;
			return ss.str();
		}
	};
public:
	Shader(const wchar_t* shaderPath, Type type);
	
	Shader::Type GetType() const { return m_Type; }
	ComPtr<ID3DBlob> GetBlob() const { return m_Blob; }
	ID3DBlob* GetBytecode() const { return m_Blob.Get(); }

private:
	ComPtr<ID3DBlob> m_Blob;
	Shader::Type m_Type;
};

