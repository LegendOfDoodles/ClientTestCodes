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
float4 OrenNayarDiffuse(float3 vLight, float3 vNormal, float3 vToCamera)
{
    float3 vToLight = normalize(vLight);
    float roguh_sq = pow(gMaterials.m_cRoughness, 2);

    float gamma = dot(vToCamera - vNormal * dot(vToCamera, vNormal),
									vToLight - vNormal * dot(vToLight, vNormal));

    float A = 1.0f - 0.5f * (roguh_sq / (roguh_sq + 0.33f));
    float B = 0.45f * (roguh_sq / (roguh_sq + 0.09f));

    float alpha = max(acos(dot(vToCamera, vNormal)), acos(dot(vToLight, vNormal)));
    float beta = min(acos(dot(vToCamera, vNormal)), acos(dot(vToLight, vNormal)));;

    float C = sin(alpha) * tan(beta);

    float3 final = (A + B * max(0.0f, gamma) * C);
    float cDiffuse = gMaterials.m_cAlbedo / PI;

    return float4(cDiffuse * max(0.0f, dot(vToLight, vNormal)) * final, 1.0f);
}

// Cook-Torrance Specular Function
float CookTorranceSpecular(float3 vLight, float3 vNormal, float3 vToCamera)
{
    float3 vToLight = normalize(vLight);
    float3 vHalf = normalize(vToCamera + vToLight);
    float metal_sq = pow(gMaterials.m_cMetalic, 2);

	// Geometrical Attenuation Factor
    float NE = dot(vNormal, vToCamera);
    float NH = dot(vNormal, vHalf);
    float NH2 = NH * NH;
    float EH = dot(vToCamera, vHalf);
    float NL = dot(vNormal, vToLight);

    float G = min(min((2 * NE * NH) / EH, (2 * NL * NH) / EH), 1.0f);

	// Microfacet Distribution Function
    float D = (metal_sq * NH2 * NH2 / 4) * exp(-1 / metal_sq * ((1 - NH2) / NH2));

	// Fresnel Reflection
    float fFri = gMaterials.m_cRefractiveIndex;

    float F = fFri + (1 - fFri) * pow((1 - NE), 5.0f);
  //  if (D== 0)
		//return 1;

    float result = (D * G * F) / (PI * NL * NE);
    return result;
}

float4 DirectionalLight(int nIndex, float3 vNormal, float3 vToCamera)
{
    float3 vToLight = -gLights[nIndex].m_vDirection;

	// Phong Specular
//    float fDiffuseFactor = dot(vToLight, vNormal);
//    float fSpecularFactor = 0.0f;
//    if (fDiffuseFactor > 0.0f)
//    {
//        if (gMaterials.m_cSepcularPower != 0.0f)
//        {
//#ifdef _WITH_REFLECT
//		float3 vReflect = reflect(-vToLight, vNormal);
//		fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials.m_cSepcularPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//            float3 vHalf = normalize(vToCamera + vToLight);
//#else
//		float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//            fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials.m_cSepcularPower);
//#endif
//        }
//    }

    return ((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * CookTorranceSpecular(vToLight, vNormal, vToCamera)));
    //return ((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * fSpecularFactor));
    //return ((gLights[nIndex].m_cAlbedo * fDiffuseFactor * gMaterials.m_cAlbedo) + (gLights[nIndex].m_cAlbedo * fSpecularFactor));
}

float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

		// Phong Specular
//        float fSpecularFactor = 0.0f;
//        float fDiffuseFactor = dot(vToLight, vNormal);
//        if (fDiffuseFactor > 0.0f)
//        {
//            if (gMaterials.m_cSepcularPower != 0.0f)
//            {
//#ifdef _WITH_REFLECT
//			float3 vReflect = reflect(-vToLight, vNormal);
//			fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials.m_cSepcularPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//                float3 vHalf = normalize(vToCamera + vToLight);
//#else
//			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//                fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials.m_cSepcularPower);
//#endif
//            }
//        }
        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        return (((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * CookTorranceSpecular(vToLight, vNormal, vToCamera))) * fAttenuationFactor);
        //return (((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * fSpecularFactor)) * fAttenuationFactor);
        //return (((gLights[nIndex].m_cAlbedo * fDiffuseFactor * gMaterials.m_cAlbedo) + (gLights[nIndex].m_cAlbedo * fSpecularFactor)) * fAttenuationFactor);
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vToCamera)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

		// Phong Specular
//        float fSpecularFactor = 0.0f;
//        float fDiffuseFactor = dot(vToLight, vNormal);
//        if (fDiffuseFactor > 0.0f)
//        {
//            if (gMaterials.m_cSepcularPower != 0.0f)
//            {
//#ifdef _WITH_REFLECT
//			float3 vReflect = reflect(-vToLight, vNormal);
//			fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterials.m_cSepcularPower);
//#else
//#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
//                float3 vHalf = normalize(vToCamera + vToLight);
//#else
//			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
//#endif
//                fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterials.m_cSepcularPower);
//#endif
//            }
//        }
#ifdef _WITH_THETA_PHI_CONES
        float fAlpha = max(dot(-vToLight, gLights[nIndex].m_vDirection), 0.0f);
        float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
#else
	float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
#endif
        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        return (((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * CookTorranceSpecular(vToLight, vNormal, vToCamera))) * fAttenuationFactor * fSpotFactor);
        //return (((gLights[nIndex].m_cAlbedo * OrenNayarDiffuse(vToLight, vNormal, vToCamera)) + (gLights[nIndex].m_cAlbedo * fSpecularFactor)) * fAttenuationFactor * fSpotFactor);
        //return (((gLights[nIndex].m_cAlbedo * fDiffuseFactor * gMaterials.m_cAlbedo) + (gLights[nIndex].m_cAlbedo * fSpecularFactor)) * fAttenuationFactor * fSpotFactor);
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 Lighting(float3 vPosition, float3 vNormal)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
    float3 vToCamera = normalize(vCameraPosition - vPosition);

    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gLights[i].m_bEnable)
        {
            if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
            {
                cColor += DirectionalLight(i, vNormal, vToCamera);
            }
            else if (gLights[i].m_nType == POINT_LIGHT)
            {
                cColor += PointLight(i, vPosition, vNormal, vToCamera);
            }
            else if (gLights[i].m_nType == SPOT_LIGHT)
            {
                cColor += SpotLight(i, vPosition, vNormal, vToCamera);
            }
        }
    }
    cColor += (gcGlobalAmbientLight * gMaterials.m_cAlbedo);
    cColor.a = gMaterials.m_cAlbedo.a;

    return (cColor);
}

