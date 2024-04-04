#include "Shader.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "Macros.h"
#include "GraphicsDebug.h"
#include "HrException.h"

Shader::Shader(const wchar_t* shaderPath, Type type)
	: m_Type(type) {

	GFX_THROW_FAILED(D3DReadFileToBlob(shaderPath, &m_Blob));
}
