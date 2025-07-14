#include "SkyBox.hlsli"


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, WVP).xyww; // スカイボックスらしい投影
    output.texCoord = input.position.xyz;
    return output;
}
