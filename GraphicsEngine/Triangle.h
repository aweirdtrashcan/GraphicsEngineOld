#pragma once

#include "IDrawable.h"

class Triangle : public IDrawable {
public:
	Triangle();
	void Rotate(float x, float y, float z, UINT frameNum);
private:
	class ConstantBuffer* m_ConstantBuffer;
	struct CBuf {
		XMFLOAT4X4 mvp;

		CBuf() {
			XMMATRIX m = XMMatrixIdentity();
			XMStoreFloat4x4(&mvp, m);
		}
	} m_CBuf{};
};

