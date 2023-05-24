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
    return frag.color;
}
