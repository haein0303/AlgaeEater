cbuffer TEST_B0 : register(b0)
{
    float4x4 gWorldViewProj;
	float4x4 gViewProj;
};

Texture2D tex_0 : register(t0);
SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 posW  : POSITION;
    float2 sizeW : SIZE;
};

struct VS_OUT
{
    float3 centerW : POSITION;
    float2 sizeW   : SIZE;
};

struct GeoOut
{
    float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float2 uv	   : TEXCOORD;
    uint   primID  : SV_PrimitiveID;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    output.centerW = input.posW;
    output.sizeW = input.sizeW;

    return output;
}

[maxvertexcount(4)]
void GS_Main(point VS_OUT gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = float3(0.0f, 0.0f, -1.0f); //gEyePosW - gin[0].CenterW;
	look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);

	float halfWidth = 0.5f * gin[0].sizeW.x;
	float halfHeight = 0.5f * gin[0].sizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].centerW + halfWidth * right - halfHeight * up, 1.0f);
	v[1] = float4(gin[0].centerW + halfWidth * right + halfHeight * up, 1.0f);
	v[2] = float4(gin[0].centerW - halfWidth * right - halfHeight * up, 1.0f);
	v[3] = float4(gin[0].centerW - halfWidth * right + halfHeight * up, 1.0f);

	float2 texC[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	GeoOut gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.posH = mul(v[i], gWorldViewProj);
		gout.posW = v[i].xyz;
		gout.normalW = look;
		gout.uv = texC[i];
		gout.primID = primID;

		triStream.Append(gout);
	}
}

float4 PS_Main(GeoOut input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);
	clip(color.a - 0.1f);
    return color;
}