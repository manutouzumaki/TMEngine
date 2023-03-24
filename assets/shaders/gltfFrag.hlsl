Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

struct PS_Input {
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
};

float4 PS_Main(PS_Input frag) : SV_TARGET {
    float3 textureColor = colorMap.Sample(colorSampler, frag.tex0.xy).rgb;

    float3 norm = normalize(frag.norm);
    float3 lightDir = float3(0.5f, 0.5f, -1.5f);
    lightDir = normalize(lightDir);

    float diff = max(dot(norm, lightDir), 0.2f);
    
    float3 color = textureColor * diff;   

    return float4(color, 1.0f);
}
