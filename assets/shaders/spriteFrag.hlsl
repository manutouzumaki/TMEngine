Texture2D colorMap : register(t0);
SamplerState colorSampler : register(s0);

cbuffer CLightBuffer : register(b1)
{
    float4 parameters[100];
    float4 colors[100];
    float3 srcAmbient;
    int count;
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

    float3 ambient = srcAmbient * textureColor.rgb;


    float constant = 1.0f;

    if(count > 0) {
        float3 output = ambient;

        for(int i = 0; i < count; ++i)
        {
            float2 position = parameters[i].xy;
            float2 lightVar = parameters[i].zw;

            float3 diffuse = colors[i].rgb * textureColor.rgb;
            
            float linear0 = lightVar.x;
            float quadratic = lightVar.y;

            float3 lightPos = float3(position.x, position.y, 1);
            float3 fragPos = frag.fragPos;
            fragPos.z = 1;

            float distance0 = length(lightPos - fragPos);
            float attenuation = 1.0 / (constant + linear0 * distance0 + quadratic * (distance0 * distance0));  
            
            diffuse  *= attenuation;

            output += diffuse;
        }

        textureColor.rgb = output;
    }

    return textureColor * frag.color;
}
