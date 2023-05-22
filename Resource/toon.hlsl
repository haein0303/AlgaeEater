float3 ToonLighting(float3 lightDir, float3 normal, float3 toonColor, float3 rimColor, float3 rimLightColor, float rimPower)
{
    // Toon Lighting
    float dotNL = saturate(dot(normal, -lightDir));
    float3 toonShade = lerp(toonColor, float3(1.0, 1.0, 1.0), dotNL);

    // Rim Lighting
    float3 worldNormal = mul(normal, (float3x3)World);
    float rim = pow(saturate(dot(worldNormal, normalize(CameraPosition - WorldPosition))), rimPower);
    float3 rimShade = lerp(rimColor, rimLightColor, rim);

    return toonShade + rimShade;
}

float4 ToonShader(VertexShaderOutput input) : SV_Target
{
    // Normal Map
    float3 normal = tex.Sample(Sampler, input.TexCoord).rgb * 2.0 - 1.0;
    normal = mul(normal, (float3x3)WorldInverseTranspose);

    // Ambient Light
    float3 ambient = AmbientColor * AmbientIntensity;

    // Directional Light
    float3 lightDir = normalize(LightDirection);
    float3 diffuse = ToonLighting(lightDir, normal, ToonColor, RimColor, RimLightColor, RimPower) * LightColor;

    // Final Color
    float4 color = float4(ambient + diffuse, 1.0);
    return color;
}