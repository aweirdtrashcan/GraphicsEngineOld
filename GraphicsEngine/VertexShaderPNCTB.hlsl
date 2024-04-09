struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 texCoord : TexCoord;
    float3 viewTangent : Tangent;
    float3 viewBitangent : Bitangent;
    float4 pos : SV_Position;
};

struct ModelCBuf
{
    matrix model;
    matrix mvp;
};

struct CameraCBuf
{
    float3 position;
    float aspectRatio;
    matrix view;
    matrix projection;
};

ConstantBuffer<ModelCBuf> modelBuffer : register(b0);
ConstantBuffer<CameraCBuf> cameraBuffer : register(b1);

VSOut main(float3 Pos : Position, float3 Normal : Normal, float3 Tangent : Tangent, float3 BiTan : Bitangent, float3 TexCoord : TexCoord)
{
    VSOut vout;
    matrix modelView = mul(modelBuffer.model, cameraBuffer.view);
    vout.viewPos = mul(float4(Pos, 1.0f), modelView);
    vout.viewNormal = mul(Pos, (float3x3) modelView);
    vout.texCoord = TexCoord;
    vout.viewTangent = mul(Tangent, (float3x3) modelView);
    vout.viewBitangent = mul(BiTan, (float3x3) modelView);
    vout.pos = mul(float4(Pos, 1.0f), modelBuffer.mvp);
    
    return vout;
}