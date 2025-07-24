//#include "Fullscreen.hlsli"

//Texture2D<float32_t4> gTexture : register(t0);
//SamplerState gSampler : register(s0);

//struct PixelShaderOutput
//{
//    float32_t4 color : SV_TARGET0;
//};

//PixelShaderOutput main(VertexShaderOutput input)
//{
//    PixelShaderOutput output;
//    output.color = gTexture.Sample(gSampler, input.texcoord);
//    float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
//    output.color.rgb = float32_t3(value, value, value);
//    return output;
    
//}


Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);

    // 輝度の加重平均（NTSC係数）
    float gray = dot(color.rgb, float3(0.299, 0.587, 0.114));

    return float4(gray, gray, gray, color.a);
}
