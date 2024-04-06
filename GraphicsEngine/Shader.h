#pragma once

#include "D3DIncls.inl"

class Shader {
public:
	enum class Type {
		VertexShader,
		PixelShader
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

