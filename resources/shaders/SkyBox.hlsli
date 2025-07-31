// 頂点シェーダ入力
struct VertexShaderInput
{
    float4 position : POSITION;
};

// 頂点シェーダ出力
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

// ピクセルシェーダ出力
struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

// マテリアル情報
struct Material
{
    float4 color;
};

cbuffer gMaterial : register(b0)
{
    Material gMaterial;
};

cbuffer gTransformationMatrix : register(b1)
{
    matrix WVP;
};
