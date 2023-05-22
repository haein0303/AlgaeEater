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

    float4x4 WV = mul(gWorld, gView);

    input.normal = float4(input.normal.xyz, 0.f);

    output.pos = mul(float4(input.pos, 1.f), mul(gWorld, mul(gView, gProjection)));

    output.viewPos = mul(float4(input.pos, 1.f), WV).xyz;
    output.viewNormal = normalize(mul(input.normal, WV).xyz);
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);
    color.z = 0.f;
    color.w = 1.f;
    float r = distance(input.uv, float2(50.5f, 50.5f));
    if (r > 50.5f) {
        color = float4(0.f, 0.f, 0.f, 0.f);
    }
    clip(color.a - 0.1f);

    return color;
}