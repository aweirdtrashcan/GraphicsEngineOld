#pragma once

#include <string>
#include <sstream>
#include "D3DIncls.inl"

enum Visibility : char {
	All,
	Vertex,
	Hull,
	Domain,
	Pixel,
	Amplification,
	Mesh
};

class TableRange {
public:
	TableRange(UINT numDescriptors, UINT baseShaderRegister, UINT registerSpace, UINT offsetInDescriptorTable)
		:
		m_NumDescriptors(numDescriptors),
		m_BaseShaderRegister(baseShaderRegister),
		m_RegisterSpace(registerSpace),
		m_OffsetInDescriptorsFromTableStart(offsetInDescriptorTable) {}

	D3D12_DESCRIPTOR_RANGE GetD3D12Range() const {
		D3D12_DESCRIPTOR_RANGE range{};
		range.RangeType = type;
		range.NumDescriptors = m_NumDescriptors;
		range.BaseShaderRegister = m_BaseShaderRegister;
		range.RegisterSpace = m_RegisterSpace;
		range.OffsetInDescriptorsFromTableStart = m_OffsetInDescriptorsFromTableStart;
		return range;
	}

	friend std::ostream& operator<<(std::ostream& stream, const TableRange& range) {
		stream << range.type << range.m_NumDescriptors << range.m_BaseShaderRegister <<
			range.m_RegisterSpace << range.m_OffsetInDescriptorsFromTableStart;
		return stream;
	}
private:
	static inline constexpr D3D12_DESCRIPTOR_RANGE_TYPE type = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	UINT m_NumDescriptors;
	UINT m_BaseShaderRegister;
	UINT m_RegisterSpace;
	UINT m_OffsetInDescriptorsFromTableStart;
};

class RootParam {
public:
	RootParam(std::string name, Visibility visibility) : m_Name(name), m_Visibility(visibility) {};
	virtual std::string GetName() const noexcept = 0;
	virtual D3D12_ROOT_PARAMETER GetD3D12Param() const noexcept = 0;

protected:
	static D3D12_SHADER_VISIBILITY ToD3DVisibility(Visibility visibility) {
		return (D3D12_SHADER_VISIBILITY)visibility;
	}

protected:
	std::string m_Name;
	Visibility m_Visibility;
};

class RootTable : public RootParam {
public:
	RootTable(TableRange range, Visibility visiblity, std::string name) 
		:
		RootParam(name, visiblity),
		m_Range(range) {}

	virtual D3D12_ROOT_PARAMETER GetD3D12Param() const noexcept override {
		D3D12_DESCRIPTOR_RANGE range = this->m_Range.GetD3D12Range();
		D3D12_ROOT_PARAMETER param{};
		param.DescriptorTable.NumDescriptorRanges = 1;
		param.DescriptorTable.pDescriptorRanges = &range;
		param.ShaderVisibility = ToD3DVisibility(m_Visibility);

		return param;
	}

	friend std::ostream& operator<<(std::ostream& stream, const RootTable& table) {
		stream << table.m_Range << (char)table.m_Visibility;
		return stream;
	}

	virtual std::string GetName() const noexcept override {
		std::stringstream ss;

		ss << *this << m_Name;

		return ss.str();
	}

private:
	TableRange m_Range;
};