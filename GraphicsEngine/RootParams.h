#pragma once

#include <string>
#include "D3DIncls.inl"

class RootParam {
protected:
	std::string name;
};

enum class RangeType : char {
	CBV,
	UAV,
	SAMPLER
};

class TableRange {
public:
	RangeType type;
	UINT numDescriptors;
	UINT baseShaderRegister;
	UINT registerSpace;
	UINT offsetInDescriptorsFromTableStart;
	
	D3D12_DESCRIPTOR_RANGE GetD3D12Range() const {
		D3D12_DESCRIPTOR_RANGE range{};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		range.NumDescriptors = numDescriptors;
		range.BaseShaderRegister = baseShaderRegister;
		range.RegisterSpace = registerSpace;
		range.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
		return range;
	}
};

enum class Visibility : char {
	Vertex,
	Pixel
};

class RootTable : public RootParam {
public:
	RootTable(TableRange range, Visibility visibility) 
		:
		range(range),
		visibility(visibility)
	{}

	D3D12_ROOT_PARAMETER GetD3D12Param() const {
		D3D12_DESCRIPTOR_RANGE range = this->range.GetD3D12Range();
		D3D12_ROOT_PARAMETER param{};
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &range;

		return param;
	}

private:
	TableRange range;
	Visibility visibility;
};