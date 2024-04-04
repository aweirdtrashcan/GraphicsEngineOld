#pragma once

#include "IBindable.h"
#include "RootSignature.h"
#include "Shader.h"

class PipelineStateObject : public IBindable {
public:
	PipelineStateObject(const RootSignature& rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
						const std::vector<Shader>& shaders);

	virtual void Bind(ID3D12GraphicsCommandList* cmdList) noexcept override;

private:
	ComPtr<ID3D12PipelineState> m_PipelineState;
};