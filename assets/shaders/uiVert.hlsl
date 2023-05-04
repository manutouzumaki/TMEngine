cbuffer CDefaultBuffer : register(b13)
{
    matrix proj;
    matrix view;
    matrix world;
    float4 color;
    float4 absUVs;
    float4 relUVs;
}

struct VS_Input
{

    float3 pos : POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : NORMAL;
};

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float4 color : TEXCOORD2;
};


PS_Input VS_Main(VS_Input vertex)
{
    PS_Input vsOut = (PS_Input)0;

    float4 worldPos = mul(float4(vertex.pos, 1.0f), world);
    vsOut.pos = mul(worldPos, view);
    vsOut.pos = mul(vsOut.pos, proj);
 
    vsOut.norm = mul(vertex.norm, (float3x3)world);
    vsOut.norm = normalize(vsOut.norm);

    vsOut.color = color;

    float uSize = absUVs.z - absUVs.x;
    float vSize = absUVs.w - absUVs.y;

    float ut = vertex.tex0.x;
    float vt = vertex.tex0.y;

    float relU = (1.0f - ut)*relUVs.x + ut*relUVs.z;
    float relV = (1.0f - vt)*relUVs.y + vt*relUVs.w;

    float u = absUVs.x + uSize * relU;
    float v = absUVs.y + vSize * relV;

    vsOut.tex0 = float2(u, v);

    return vsOut;
}
