struct PSIn
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 texCoord : TexCoord;
    float3 viewTangent : Tangent;
    float3 viewBitangent : Bitangent;
};

struct CameraCBuf
{
    float3 position;
    float aspectRatio;
    matrix view;
    matrix projection;
};

ConstantBuffer<CameraCBuf> cbuf : register(b1);

static const float3 mLightPos = float3(0.0f, 0.0f, -20.f);

float4 main(PSIn psin) : SV_Target
{
    float3 vLightPos = mul(mLightPos, (float3x3) cbuf.view);
    
    float3 vToL = vLightPos - psin.viewPos;
    psin.viewNormal = normalize(psin.viewNormal);
    
    float3 ndotl = dot(normalize(vToL), psin.viewNormal);
    
    float3 color = { 1.0f, 1.0f, 1.0f };
    
    return float4(color * ndotl, 1.0f);
}