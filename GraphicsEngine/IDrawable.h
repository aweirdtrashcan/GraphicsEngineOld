#pragma once

#include "IBindable.h"
#include <memory>

class IDrawable {
public:
	void RecordDrawCommands(UINT frameNum, ID3D12GraphicsCommandList* cmdList) const noexcept(!IS_DEBUG);

protected:
	void AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG);

private:
	std::vector<std::shared_ptr<IBindable>> m_Bindables;
	UINT m_IndicesCount = 0;
};

