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
    float4x4 gProj;
    LightInfo lightInfo;
    float4x4 gTexTransform;
    float4x4 gMatTransform;
    float4x4 gBoneTransforms[96];
};

Texture2D tex_0 : register(t0);
SamplerState sam_0 : register(s0);

struct VS_IN
{
    float3 pos    : POSITION;
    float4 normal : NORMAL;
    float2 uv    : TEXCOORD;
    float3 TangentL : TANGENT;
    float3 BoneWeights : WEIGHTS;
    uint4 BoneIndices  : BONEINDICES;
};

struct VS_OUT
{
    float4 pos    : SV_POSITION;
    float3 viewNormal : NORMAL;
    float2 uv    : TEXCOORD;
    float3 viewPos : POSITION;
};


//TODO
//����Ʈ�� �������� �Ӹ����� �ø��� �ʹ�
LightInfo CalculateLightColor(float3 viewNormal, float3 viewPos)
{
    //������ ���� ���̶��
    LightInfo color = (LightInfo)0.f;

    float3 viewLightDir;
    float4 posi = float4(1.0f, 1.0f, 1.0f,1.0f);
    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;

    float4x4 transformationMatrix = float4x4(
        1.0, 0.0, 0.0, 0.0,   // X�� ��ȯ
        0.0, 1.0, 0.0, 0.0,   // Y�� ��ȯ
        0.0, 0.0, 1.0, 0.0,   // Z�� ��ȯ
        0.3, 5.0, 0.0, 1.0    // �̵� ��ȯ
        );

    // Directional Light
    //viewLightDir = normalize(mul(float4(lightInfo.direction.xyz, 0.f), gView).xyz);
    
    viewLightDir = normalize(mul(mul(transformationMatrix,gWorld) ,posi).xyz);
    //viewLightDir = normalize(mul(gWorld, posi).xyz);
   

    //viewLightDir = normalize(mul(gWorld, float4(posi, 1.0f)).xyz);
    //viewLightDir = gWorld;
    
   

    diffuseRatio = saturate(dot(-viewLightDir, viewNormal));
    

    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, viewNormal)) * viewNormal));
    float3 eyeDir = normalize(viewPos);

    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);

    //��ǻ� ����ũ�ϰ� ������ �ǰ�, �������� �״�? ������ ���Ǵµ�, �ϴ� ��ǻ� ���Ǻη� ������ �峭�� �غ���
    if (0.98f < length(lightInfo.diffuse * diffuseRatio * distanceRatio)) {
        color.diffuse = ceil(lightInfo.diffuse * diffuseRatio * distanceRatio * 2.5) / 1.0f;
    }
    else {
        color.diffuse = lightInfo.diffuse * diffuseRatio * distanceRatio;
    }

    //color.diffuse = ceil(lightInfo.diffuse * diffuseRatio * distanceRatio * 2.5) / 1.0f;
    color.ambient = ceil(lightInfo.ambient * distanceRatio * 2.5) / 2.0f;
    color.specular = ceil(lightInfo.specular * specularRatio * distanceRatio * 2.5) / 2.0f;

    return color;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0.0f;

    //�ִϸ��̼�
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = input.BoneWeights.x;
    weights[1] = input.BoneWeights.y;
    weights[2] = input.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 4; ++i)
    {
        posL += weights[i] * mul(float4(input.pos, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz;
        tangentL += weights[i] * mul(input.TangentL.xyz, (float3x3)gBoneTransforms[input.BoneIndices[i]]);
    }

    input.pos = posL;
    input.normal = float4(input.normal.xyz, 0.f);
    input.TangentL.xyz = tangentL;

    output.pos = mul(float4(input.pos, 1.f), mul(gWorld, mul(gView, gProj)));
    output.viewNormal = input.normal;
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = tex_0.Sample(sam_0, input.uv);

    LightInfo totalColor = (LightInfo)0.f;
    LightInfo lightColor = CalculateLightColor(input.viewNormal, input.viewNormal);

    //lightColor.diffuse.xyz = ceil(saturate(lightColor.diffuse.xyz) * 1.5) / 2.0f;
    lightColor.diffuse.xyz = saturate(lightColor.diffuse.xyz);

    totalColor.diffuse += lightColor.diffuse;
    totalColor.ambient += lightColor.ambient;
    totalColor.specular += lightColor.specular;

    color.xyz = (totalColor.diffuse.xyz * color.xyz) + totalColor.ambient.xyz * color.xyz + totalColor.specular.xyz;

    return color;
}