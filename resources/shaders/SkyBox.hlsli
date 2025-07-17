// ���_�V�F�[�_����
struct VertexShaderInput
{
    float4 position : POSITION;
};

// ���_�V�F�[�_�o��
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 texCoord : TEXCOORD;
};

// �s�N�Z���V�F�[�_�o��
struct PixelShaderOutput
{
    float4 color : SV_TARGET;
};

// �}�e���A�����
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
