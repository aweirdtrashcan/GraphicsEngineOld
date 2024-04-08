struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

struct CBuf
{
    matrix mvp;
};

ConstantBuffer<CBuf> cbuf : register(b0);

VSOut main(float3 pos : Position, float3 color : Color)
{
    VSOut vout;
    vout.pos = mul(float4(pos, 1.0f), cbuf.mvp);
    vout.color = color;
	return vout;
}