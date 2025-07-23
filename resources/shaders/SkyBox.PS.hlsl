#include "SkyBox.hlsli"

TextureCube<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 texColor = gTexture.Sample(gSampler, input.texCoord);
    output.color = texColor * gMaterial.color;
    return output;
}
