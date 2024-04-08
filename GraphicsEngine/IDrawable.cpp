#include "IDrawable.h"

#include "IndexBuffer.h"
#include <exception>
#include <pix3.h>

void IDrawable::RecordDrawCommands(UINT frameNum, ID3D12GraphicsCommandList* cmdList) const noexcept(!IS_DEBUG) {
	//GFX_THROW_FAILED(m_CommandAllocator->Reset());
	//GFX_THROW_FAILED(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
#ifdef PIX_DEBUG
	PIXBeginEvent(PIX_COLOR_DEFAULT, "Start Drawable Binds");
#endif
	m_Bindables[0]->SetFrameNum(frameNum);
	for (const std::shared_ptr<IBindable>& b : m_Bindables) {
		b->Bind(cmdList);
	}
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
