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
    float4 color : COLOR;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float4 color : COLOR;
};

PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;

    float4 viewPos = mul(float4(vertex.pos, 1.0f), view);
    vsOut.pos = mul(viewPos, proj);
    vsOut.tex0 = vertex.tex0;
    vsOut.color = vertex.color;

    return vsOut;
}
