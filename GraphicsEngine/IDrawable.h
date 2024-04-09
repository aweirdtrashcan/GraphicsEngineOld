#pragma once

#include "IBindable.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <memory>

class IDrawable {
public:
	void RecordDrawCommands(UINT frameNum, ID3D12GraphicsCommandList* cmdList, Camera& camera) noexcept(!IS_DEBUG);
	void Rotate(float x, float y, float z) noexcept(!IS_DEBUG);
	void Move(float x, float y, float z) noexcept(!IS_DEBUG);
	void Update(const Camera& camera, UINT frameNum);
protected:
	void AddBind(std::shared_ptr<IBindable> bind) noexcept(!IS_DEBUG);

protected:
	struct CBuf {
		XMFLOAT4X4 model;
		XMFLOAT4X4 mvp;

		CBuf() {
			XMMATRIX m = XMMatrixIdentity();
			XMStoreFloat4x4(&model, m);
			XMStoreFloat4x4(&mvp, m);
		}
	};

	CBuf m_CBuf{};
	std::shared_ptr<ConstantBuffer> m_ConstantBuffer;
	bool m_HasConstantBuffer = false;
private:
	XMFLOAT3 m_Rotation = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };
	std::vector<std::shared_ptr<IBindable>> m_Bindables;
	UINT m_IndicesCount = 0;
};

