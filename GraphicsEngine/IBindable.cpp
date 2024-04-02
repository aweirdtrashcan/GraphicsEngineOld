#include "IBindable.h"

#include "GraphicsFabric.h"

IBindable::IBindable(ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandAllocator> commandAllocator) 
	:
	m_CommandList(commandList),
	m_CommandAllocator(commandAllocator)
{}
