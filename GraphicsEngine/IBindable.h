#pragma once

#include "D3DIncls.inl"

class IBindable {
public:
	virtual ~IBindable() = default;
	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) = 0;
};