#pragma once

#include "IBindable.h"

class RootSignature : public IBindable {
	friend class PipelineStateObject;
public:
	RootSignature();
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;
};

