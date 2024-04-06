#pragma once

#include "IBindable.h"
#include "RootSignature.h"
#include "Shader.h"

class PipelineStateObject : public IBindable {
public:
	PipelineStateObject(const RootSignature& rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
						const std::vector<Shader>& shaders);

	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

	static std::shared_ptr<PipelineStateObject> Resolve(const RootSignature& rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
														const std::vector<Shader>& shaders);

	static std::string GenerateKey(const RootSignature& rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
								   const std::vector<Shader>& shaders, std::string key = "");

private:
	ComPtr<ID3D12PipelineState> m_PipelineState;
};