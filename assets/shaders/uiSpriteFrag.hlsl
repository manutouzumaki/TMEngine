Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float4 color : TEXCOORD2;
};

float4 PS_Main(PS_Input frag) : SV_TARGET
{
    float4 textureColor = colorMap.Sample(colorSampler, frag.tex0.xy);
    return textureColor * float4(frag.color.xyz, 1.0f);
}