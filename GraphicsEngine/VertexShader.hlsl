struct VSOut
{
    float3 color : COLOR;
    float4 pos : SV_Position;
};

struct CameraCBuf
{
    float4 position;
    matrix view;
    matrix projection;
    float aspectRatio;
};

ConstantBuffer<CameraCBuf> camera : register(b1);

VSOut main(float3 pos : Position, float3 color : Color)
{
    VSOut vout;
    vout.pos = float4(pos, 1.0f);
    vout.color = color;
    return vout;
}