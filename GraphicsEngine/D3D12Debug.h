#pragma once

#include <directx/d3dx12.h>

#if IS_DEBUG
class D3D12Debug {
public:
	D3D12Debug(ID3D12Device* device);
};
#endif

