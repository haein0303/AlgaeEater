struct LightInfo
{
    float4 diffuse;
    float4 ambient;
    float4 specular;
    float4 direction;
};

cbuffer TEST_B0 : register(b0)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gProjection;
    LightInfo lightInfo;
    float4x4 gTexTransform;
    float4x4 gMatTransform;
    float4x4 gBoneTransforms[96];
    int object_type;
    float4 camera_pos;
};

Texture2D tex_0 : register(t0);
SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 viewNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float4 viewPos = mul(float4(input.pos, 0), gView);
    float4 clipSpacePos = mul(viewPos, gProjection);
    output.pos = clipSpacePos.xyww;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);

    return color;
}