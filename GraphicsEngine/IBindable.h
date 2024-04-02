#pragma once

#include "D3DIncls.inl"

class IBindable {
public:
	IBindable(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator);
	virtual ~IBindable() = default;
	virtual void Bind() noexcept = 0;

protected:
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
};