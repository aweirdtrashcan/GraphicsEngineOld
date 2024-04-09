#include "Camera.h"

#include "ConstantBuffer.h"
#include "Graphics.h"

Camera::Camera(float viewWidth, float viewHeight, UINT cameraCBufSlot)
	:
	m_CameraSlot(cameraCBufSlot) {

	m_CameraBuffer.aspectRatio = viewWidth / viewHeight;
	m_CameraBuffer.position = { 0.0f, 0.0f, -4.0f, 1.0f };

	m_ConstantBuffer = new ConstantBuffer(sizeof(CameraCBuf), Graphics::s_BufferCount, m_CameraSlot);
}

Camera::~Camera() {
	delete m_ConstantBuffer;
}

void Camera::Update(UINT frameNum) {
	XMVECTOR eyePos = XMLoadFloat4(&m_CameraBuffer.position);

	XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	XMVECTOR camDirection = XMVector4Transform(
		focus,
		XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, 0.0f)
	);
	XMVECTOR lookDirection = XMVectorAdd(eyePos, camDirection);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX view = XMMatrixLookAtLH(eyePos, lookDirection, up);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(45.f, m_CameraBuffer.aspectRatio, 0.1f, 1000.f);

	XMStoreFloat4x4(&m_CameraBuffer.view, view);
	XMStoreFloat4x4(&m_CameraBuffer.projection, projection);
	m_ConstantBuffer->Update(&m_CameraBuffer, sizeof(m_CameraBuffer), frameNum);
}

void Camera::Bind(ID3D12GraphicsCommandList* commandList, UINT frameNum) {
	m_ConstantBuffer->Bind(commandList, frameNum);
}

XMMATRIX Camera::GetViewProjectionMatrix() const {
	XMMATRIX view = XMLoadFloat4x4(&m_CameraBuffer.view);
	XMMATRIX projection = XMLoadFloat4x4(&m_CameraBuffer.projection);

	return view * projection;
}
