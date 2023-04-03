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
    float4 uvs : UVS;
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

    float ut = vertex.tex0.x;
    float u = (1.0f - ut)*vertex.uvs.x + ut*vertex.uvs.z;
    float vt = vertex.tex0.y;
    float v = (1.0f - vt)*vertex.uvs.y + vt*vertex.uvs.w;
    vsOut.tex0 = float2(u, v);

    float4 worldPos = mul(float4(vertex.pos, 1.0f), vertex.world);
    vsOut.pos = mul(worldPos, view);
    vsOut.pos = mul(vsOut.pos, proj);
    
    vsOut.color = vertex.color;

    return vsOut;
}
