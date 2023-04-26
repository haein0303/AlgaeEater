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
    float4 hp_bar_start_pos;
    int current_hp;
    float hp_bar_size;
    int max_hp;
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
    input.pos.x = input.pos.x * current_hp / max_hp + (float)(max_hp - current_hp) / max_hp;

    output.pos = mul(float4(input.pos, 1.f), mul(gWorld , mul(gView, gProjection)));

    output.viewPos = mul(float4(input.pos, 1.f), WV).xyz;
    output.viewNormal = normalize(mul(input.normal, WV).xyz);
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);

    return color;
}