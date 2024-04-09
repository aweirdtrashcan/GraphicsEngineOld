#include "IDrawable.h"

#include "IndexBuffer.h"
#include <exception>
#include <pix3.h>

void IDrawable::RecordDrawCommands(UINT frameNum, ID3D12GraphicsCommandList* cmdList, Camera& camera) noexcept(!IS_DEBUG) {
#ifdef PIX_DEBUG
	PIXBeginEvent(PIX_COLOR_DEFAULT, "Start Drawable Binds");
#endif
	for (const std::shared_ptr<IBindable>& b : m_Bindables) {
		b->Bind(cmdList, frameNum);
	}
	camera.Bind(cmdList, frameNum);
	cmdList->DrawIndexedInstanced(m_IndicesCount, 1, 0, 0, 0);

#ifdef PIX_DEBUG
	PIXEndEvent();
#endif
}

void IDrawable::Rotate(float x, float y, float z) noexcept(!IS_DEBUG) {
	m_Rotation.x = x;
	m_Rotation.y = y;
	m_Rotation.z = z;
}

void IDrawable::Move(float x, float y, float z) noexcept(!IS_DEBUG) {
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void IDrawable::Update(const Camera& camera, UINT frameNum) {
	if (!m_HasConstantBuffer) return;
	XMMATRIX model = XMLoadFloat4x4(&m_CBuf.model) *
		XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z) *
		XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX viewProjection = camera.GetViewProjectionMatrix();

	CBuf cbuf = m_CBuf;
	XMStoreFloat4x4(&cbuf.model, XMMatrixTranspose(model));
	XMStoreFloat4x4(&cbuf.mvp, XMMatrixTranspose(model * viewProjection));

	m_ConstantBuffer->Update(&cbuf, sizeof(cbuf), frameNum);
}

void IDrawable::AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG) {
#if IS_DEBUG
	if (bind == nullptr) throw std::exception("Trying to bind a null pointer");
#endif
	if (IndexBuffer* ib = dynamic_cast<IndexBuffer*>(bind.get())) {
		m_IndicesCount = ib->GetIndicesCount();
	}
	if (std::shared_ptr<ConstantBuffer> cb = std::dynamic_pointer_cast<ConstantBuffer>(bind)) {
		m_ConstantBuffer = cb;
		m_HasConstantBuffer = true;
	}
	m_Bindables.push_back(std::move(bind));
}
