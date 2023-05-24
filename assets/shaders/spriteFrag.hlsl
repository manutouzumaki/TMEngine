Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer CLightBuffer : register(b1)
{
    float2 lightPosition;
    float2 range;
}

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float4 color : TEXCOORD2;
    float3 fragPos : TEXCOORD3;
};

float4 PS_Main(PS_Input frag) : SV_TARGET
{


    float4 textureColor = colorMap.Sample(colorSampler, frag.tex0.xy);

    float3 ambient = float3(0.4, 0.4, 0.4) * textureColor.rgb;
    float3 diffuse = float3(1, 0.5, 0.8) * textureColor.rgb;

    float3 lightPos = float3(10, 10, 1);
    float3 fragPos = frag.fragPos;
    fragPos.z = 1;

    float constant = 0.5f;
    float linear0 = 0.7;
    float quadratic = 3.0f;

    float distance0 = length(lightPos - fragPos);
    float attenuation = 1.0 / (constant + linear0 * distance0 + quadratic * (distance0 * distance0));  

    //ambient  *= attenuation; 
    diffuse  *= attenuation;


    float3 finalColor = ambient + diffuse;

    textureColor.rgb = finalColor;

    return textureColor;
}
