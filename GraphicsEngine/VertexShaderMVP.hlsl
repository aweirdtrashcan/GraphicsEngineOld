struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

struct ModelBuffer
{
    matrix model;
};

struct GlobalBuffer
{
    matrix view;
    matrix projection;
};

ConstantBuffer<ModelBuffer> modelBuffer : register(b0);
ConstantBuffer<GlobalBuffer> globalBuffer : register(b1);

VSOut main(float3 pos : Position, float3 color : Color)
{
    VSOut vout;
    matrix mvp = mul(modelBuffer.model, mul(globalBuffer.view, globalBuffer.projection));
    vout.pos = mul(float4(pos, 1.0f), mvp);
    vout.color = color;
	return vout;
}