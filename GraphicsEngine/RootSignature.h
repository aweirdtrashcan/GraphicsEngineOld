#pragma once

#include "IBindable.h"
#include "RootParams.h"

class RootSignature : public IBindable {
	friend class PipelineStateObject;
public:
	RootSignature(const RootParam& param);
	RootSignature();
	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept(!IS_DEBUG) override;

	static std::shared_ptr<RootSignature> Resolve(const RootParam& param);
	static std::shared_ptr<RootSignature> Resolve();
	static std::string GenerateKey(const RootParam& param);
	static std::string GenerateKey();

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;

};

