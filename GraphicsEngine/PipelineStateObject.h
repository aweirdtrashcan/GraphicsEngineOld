#pragma once

#include "IBindable.h"
#include "RootSignature.h"
#include "Shader.h"
#include <unordered_map>

class PipelineStateObject : public IBindable {
public:
	enum Option {
		EMPTY_ROOT_SIG_PS__COLOR_VS__POS_COLOR,
		MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR
	};
public:
	PipelineStateObject(ComPtr<ID3D12RootSignature> rootSignature, const std::vector<D3D12_INPUT_ELEMENT_DESC>& elements,
						const std::vector<Shader>& shaders);

	virtual void Bind(ID3D12GraphicsCommandList* cmdList, UINT frameNumber) noexcept(!IS_DEBUG) override;

	static std::shared_ptr<PipelineStateObject> Resolve(PipelineStateObject::Option option);

private:
	static void BuildAllPipelineStates();
	static void Build_EMPTY_ROOT_SIG_PS__COLOR_VS__POS_COLOR(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied,
															 const std::vector<Shader>& shaders);
	static void Build_MVP_DESCRIPTOR_TABLE_PS__COLOR_VS__POS_COLOR(const std::vector<D3D12_INPUT_ELEMENT_DESC>& ied,
																   const std::vector<Shader>& shaders);

private:
	ComPtr<ID3D12PipelineState> m_PipelineState;
	ComPtr<ID3D12RootSignature> m_RootSignature;
	static inline std::unordered_map<PipelineStateObject::Option, std::shared_ptr<PipelineStateObject>> m_PipelineCaches;
};