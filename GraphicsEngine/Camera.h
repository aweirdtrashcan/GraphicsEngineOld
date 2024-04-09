#pragma once

#include "D3DIncls.inl"

class Camera {
public:
	Camera(float viewWidth, float viewHeight, UINT cameraCBufSlot = 1);
	~Camera();
	void Update(UINT frameNum);
	void Bind(ID3D12GraphicsCommandList* commandList, UINT frameNum);
	XMMATRIX GetViewProjectionMatrix() const;

private:
	struct CameraCBuf {
		XMFLOAT4 position;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		float aspectRatio;
	};

private:
	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	UINT m_CameraSlot;
	CameraCBuf m_CameraBuffer;
	class ConstantBuffer* m_ConstantBuffer;
};

