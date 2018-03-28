//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			8 
#define MAX_MATERIALS		1

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

static float PI = 3.1415926535f;

struct MATERIAL
{
    float4 m_cAlbedo;
    float m_cSepcularPower;
    float m_cRoughness;
    float m_cMetalic;
    float m_cRefractiveIndex;
};

struct LIGHT
{
    float4 m_cAlbedo;
    float3 m_vPosition;
    float m_fFalloff;
    float3 m_vDirection;
    float m_fTheta; //cos(m_fTheta)
    float3 m_vAttenuation;
    float m_fPhi; //cos(m_fPhi)
    bool m_bEnable;
    int m_nType;
    float m_fRange;
    float padding;
};

cbuffer cbLights : register(b3)
{
    LIGHT gLights[MAX_LIGHTS];
    float4 gcGlobalAmbientLight;
};

cbuffer cbMaterial : register(b4)
{
    MATERIAL gMaterials;
};

// OrenNayar Diffuse Function
float4 OrenNayarDiffuse(float3 vLight, float3 vNormal, float3 vCamera)
{
    float3 L = normalize(vLight);
    float3 N = vNormal;
    float3 V = normalize(vCamera);

    half LN = dot(L, N);
    half VN = dot(V, N);
    half result = saturate(LN);
    half soft_rim = saturate(1 - VN / 2);
    half fakey = pow(1 - result * soft_rim, 2);
    half fakey_magic = 0.62;
    fakey = fakey_magic - fakey * fakey_magic;

    return lerp(result, fakey, gMaterials.m_cRoughness);
}

// Cook-Torrance Specular Function
float4 CookTorranceSpecular(float3 vLight, float3 vNormal, float3 vCamera)
{
    float3 L = normalize(vLight);
    float3 N = vNormal;
    float3 V = normalize(vCamera);
    float3 H = normalize(L + V);

    float NV = dot(N, V);
    float NH = dot(N, H);
    float VH = dot(V, H);
    float NL = dot(N, L);
    float LH = dot(L, H);

    float M2 = gMaterials.m_cMetalic * gMaterials.m_cMetalic;
    float NH2 = NH * NH;
    float D;
    if (M2 != 0)
        D = exp(-(1 - NH2) / (NH2 * M2)) / (4 * M2 * NH2 * NH2);
	else
        return float4(1.0f, 1.0f, 1.0f, 1.0f);

    float G = min(1, min(2 * NH * NV / VH, 2 * NH * NL / VH));

    float n = 20.0f;
    float g = sqrt(n * n + LH * LH - 1);
    float gpc = g + LH;
    float gnc = g - LH;
    float cgpc = LH * gpc - 1;
    float cgnc = LH * gnc + 1;
    float F = 0.5f * gnc * gnc * (1 + cgpc * cgnc / (cgnc * cgnc)) / (gpc * gpc);

    float4 ks = { 2.0f * 0.486f, 2.0f * 0.486f, 2.0f * 0.185f, 1.0f };

    return gMaterials.m_cAlbedo + ks * max(0, F * D * G / NV);
}

float4 DirectionalLight(int nIndex, float3 vNormal, float3 vCamera)
{
    float3 vToLight = -gLights[nIndex].m_vDirection;

    return gLights[nIndex].m_cAlbedo * (OrenNayarDiffuse(vToLight, vNormal, vCamera) + CookTorranceSpecular(vToLight, vNormal, vCamera));
}

float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vCamera)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        if (fAttenuationFactor != 0)
            return gLights[nIndex].m_cAlbedo * (OrenNayarDiffuse(vToLight, vNormal, vCamera) + CookTorranceSpecular(vToLight, vNormal, vCamera)) * fAttenuationFactor;
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vCamera)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

#ifdef _WITH_THETA_PHI_CONES
        float fAlpha = max(dot(normalize(-vToLight), gLights[nIndex].m_vDirection), 0.0f);
        float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
#else
	float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
#endif
        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        if (fSpotFactor != 0 && fAttenuationFactor != 0)
            return gLights[nIndex].m_cAlbedo * (OrenNayarDiffuse(vToLight, vNormal, vCamera) + CookTorranceSpecular(vToLight, vNormal, vCamera)) * fAttenuationFactor * fSpotFactor;
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 Lighting(float3 vPosition, float3 vNormal)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
    float3 vCamera = vCameraPosition - vPosition;

    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gLights[i].m_bEnable)
        {
            if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
            {
                cColor += DirectionalLight(i, vNormal, vCamera);
            }
            else if (gLights[i].m_nType == POINT_LIGHT)
            {
                cColor += PointLight(i, vPosition, vNormal, vCamera);
            }
            else if (gLights[i].m_nType == SPOT_LIGHT)
            {
                cColor += SpotLight(i, vPosition, vNormal, vCamera);
            }
        }
    }
    cColor += (gcGlobalAmbientLight * gMaterials.m_cAlbedo);
    cColor.a = gMaterials.m_cAlbedo.a;

    return (cColor);
}

