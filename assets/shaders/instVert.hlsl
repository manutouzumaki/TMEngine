cbuffer CBuffer : register(b0)
{
    matrix proj;
    matrix view;
    matrix world;
}

struct VS_Input
{
    float3 pos : POSITION;
    float2 tex0 : TEXCOORD0;
    float4x4 world: WORLD;
    float4 color: COLOR;
    uint InstanceId: SV_InstanceID;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float4 color: COLOR;
};

PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;

    float4 worldPos = mul(float4(vertex.pos, 1.0f), vertex.world);
    vsOut.pos = mul(worldPos, view);
    vsOut.pos = mul(vsOut.pos, proj);
    vsOut.tex0 = vertex.tex0;
    vsOut.color = vertex.color;

    return vsOut;
}
