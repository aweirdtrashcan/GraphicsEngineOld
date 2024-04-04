#pragma once

#include "IBindable.h"
#include <memory>

class IDrawable {
public:
	ID3D12CommandList* RecordDrawCommands() const noexcept(!IS_DEBUG);
	void PrepareToDraw() noexcept(!IS_DEBUG);

	void __Debug_SetCommandList(ID3D12GraphicsCommandList* cmdList) {
		m_CommandList = cmdList;
	}

protected:
	void AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG);

protected:
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator;

private:
	std::vector<std::shared_ptr<IBindable>> m_Bindables;
	UINT m_IndicesCount = 0;
};

