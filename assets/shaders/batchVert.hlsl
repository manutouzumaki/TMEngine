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
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;

    float4 viewPos = mul(float4(vertex.pos, 1.0f), view);
    vsOut.pos = mul(viewPos, proj);
    vsOut.tex0 = vertex.tex0;

    return vsOut;
}
