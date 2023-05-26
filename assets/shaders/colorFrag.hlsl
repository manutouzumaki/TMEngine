struct PointLight {
    float3 attributes;
    float range;
    float3 color;
    float pad0;
    float2 position;
    float pad1;
    float pad2;
};

cbuffer CLightBuffer : register(b1)
{
    float3 srcAmbient;
    int count;
    PointLight lights[100];
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

    float4 textureColor = frag.color;

    float3 ambient = srcAmbient * textureColor.rgb;

    if(count > 0) {
        float3 output = ambient;

        for(int i = 0; i < count; ++i)
        {
            float2 position   = lights[i].position;
            float3 lightPos = float3(position.x, position.y, 1);
            float3 fragPos = frag.fragPos;
            fragPos.z = 1;
            float distance0 = length(lightPos - fragPos);

            float range = lights[i].range;
            if(distance0 > range) continue;

            float3 attributes = lights[i].attributes;
            float3 color      = lights[i].color;

            float3 diffuse = color;
            
            float constant  = attributes.x;
            float linear0   = attributes.y;
            float quadratic = attributes.z;

            float attenuation = 1.0 / (constant + linear0 * distance0 + quadratic * (distance0 * distance0));  
            
            diffuse  *= attenuation;

            output += diffuse;
        }

        textureColor.rgb = output;
    }

    return textureColor;


}
