Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

struct PS_Input {
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};

float4 PS_Main(PS_Input frag) : SV_TARGET {
    float3 textureColor = colorMap.Sample(colorSampler, frag.tex0.xy).rgb;
    return float4(textureColor, 1.0f);
}
