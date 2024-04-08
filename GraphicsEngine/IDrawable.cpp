#include "IDrawable.h"

#include "IndexBuffer.h"
#include <exception>
#include <pix3.h>
#include "ConstantBuffer.h"

void IDrawable::RecordDrawCommands(UINT frameNum, ID3D12GraphicsCommandList* cmdList) const noexcept(!IS_DEBUG) {
#ifdef PIX_DEBUG
	PIXBeginEvent(PIX_COLOR_DEFAULT, "Start Drawable Binds");
#endif
	for (const std::shared_ptr<IBindable>& b : m_Bindables) {
		b->Bind(cmdList, frameNum);
	}
	Graphics::GetGlobalConstantBuffer()->Bind(cmdList, frameNum);
	cmdList->DrawIndexedInstanced(m_IndicesCount, 1, 0, 0, 0);

#ifdef PIX_DEBUG
	PIXEndEvent();
#endif
}

void IDrawable::AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG) {
#if IS_DEBUG
	if (bind == nullptr) throw std::exception("Trying to bind a null pointer");
#endif
	if (IndexBuffer* ib = dynamic_cast<IndexBuffer*>(bind.get())) {
		m_IndicesCount = ib->GetIndicesCount();
	}
	m_Bindables.push_back(std::move(bind));
}
