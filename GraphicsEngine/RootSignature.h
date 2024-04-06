#pragma once

#include "IBindable.h"
#include "ISerializable.h"

class RootSignature : public IBindable, public ISerializable {
	friend class PipelineStateObject;
public:
	RootSignature();
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;

	// Inherited via ISerializable
	virtual const char* Serialize() const noexcept override;
};

