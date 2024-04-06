#include "IDrawable.h"

#include "IndexBuffer.h"
#include <exception>
#include <pix3.h>

ID3D12CommandList* IDrawable::RecordDrawCommands() const noexcept(!IS_DEBUG) {
	GFX_THROW_FAILED(m_CommandAllocator->Reset());
	GFX_THROW_FAILED(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
#ifdef PIX_DEBUG
	PIXBeginEvent(PIX_COLOR_DEFAULT, "Start Drawable Binds");
#endif
	for (const std::shared_ptr<IBindable>& b : m_Bindables) {
		b->Bind(m_CommandList.Get());
	}
	m_CommandList->DrawIndexedInstanced(m_IndicesCount, 1, 0, 0, 0);

#ifdef PIX_DEBUG
	PIXEndEvent();
#endif

	return m_CommandList.Get();
}

void IDrawable::PrepareToDraw() noexcept(!IS_DEBUG) {
	GFX_THROW_FAILED(m_CommandAllocator->Reset());
	GFX_THROW_FAILED(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));
}

void IDrawable::AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG) {
	if (bind == nullptr) throw std::exception("Trying to bind a null pointer");
	if (IndexBuffer* ib = dynamic_cast<IndexBuffer*>(bind.get())) {
		m_IndicesCount = ib->GetIndicesCount();
	}
	m_Bindables.push_back(std::move(bind));
}
